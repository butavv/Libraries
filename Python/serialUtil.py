
import serial
import serial.tools.list_ports
from threading import Thread
import sys
import glob
import time
#from app import handlePacket


DEBUG = False

#Serial Globals
mPort		= None
mChatting 	= False
mLock 		= False
mInBuffer 	= []
mOutBuffer	= []

#Packet Handler Globals
mBytes                      = 0
startFlag                   = False
escapeFlag                  = False
addToPacket                 = False
checksum                    = 0
checksumIndex               = 0
packetIn                    = []
packetOut                   = []

STX                         = 15
ETX                         = 4
DLE                         = 5

# Find peripherals and open serial ports
# Returns list of established port names
def getSerialPort(portName, baudRate, timeout):

	global mPort

	portFound = False
	result = None
	
	#Support Windows
	if sys.platform.startswith('win'):
		ports = ['COM%s' % (i + 1) for i in range(256)]

	#Support Linux	
	elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
		ports = glob.glob('/dev/tty[A-Za-z]*')
	elif sys.platform.startswith('darwin'):
		ports = glob.glob('/dev/tty.*')
	else:
		raise EnvironmentError('Unsupported Platform')

	
	for port in ports:
		try:

			#Found Port
			if port == portName:
				port = serial.Serial(port, baudRate, timeout=timeout)
				portFound = True
				#print("Found QR Port")
				sys.stdout.flush()
				result = port


		except(OSError, serial.SerialException):
			pass


	#Validate port was found
	if not portFound:
		print (portName+" not found.")
		mPort = serial.Serial()
		result = None
	
	''''''

	return result


def serialWriter():

	global mOutBuffer
	global mLock
	global mChatting
	global mPort

	print("Serial Writer Thread started...")
	sys.stdout.flush()

	while mChatting:
		try:
			if not mPort.is_open:
				print("Serial Connection Closed")
				mChatting = false
			else:
				txData = []
				write = False
				for byte in mOutBuffer: 
					#print("Serial byte "+str(byte))
					#sys.stdout.flush()
					txData.append(byte)
					if byte == "\r":
						txData.pop()
						#print("SERIAL OUT: "+str(txData)+"\n")
						#sys.stdout.flush()
						mLock = True
						mPort.write(txData)
						mLock = False
						txData = []
						write = True
				if write:		
					mOutBuffer = []

		except Exception:
			print("Failed writing to serial port")
			traceback.print_exc()
			sys.stdout.flush()
			mChatting = False


def waitForLock(timeout=10000):
	global mLock

	count = 0
	while(mLock == True):
		time.sleep(0.01)
		count += 1
		if( count > timeout):
			return False

	return True

def serialReader():

	global mPort
	global mChatting
	global mInBuffer
	global mLock
	global mChatting

	
	print("Serial Reader Thread started...")
	sys.stdout.flush()

	while mChatting:

		#bytesAvailable = serialAvailable()
		#if bytesAvailable > 0:

			#for byte in range(0, bytesAvailable):
		try:
			if not mPort.is_open:
				print("Serial Connection Closed")
				mChatting = false
			else:

				if mLock == False:

					#if mPort.in_waiting > 0:
						#mLock = True
					byteIn = mPort.read()
						#mLock = False

					if byteIn is not None:
						try:
							handleIncomingByte(ord(byteIn))
							#time.sleep(0.001)
						except:
							#print("Could not parse byte")
							#sys.stdout.flush()
							#time.sleep(0.001)
							pass
					else:
						print("Received none type")
						sys.stdout.flush()
						#if len(mInBuffer) > 0:
						#	print(mInBuffer)
						#	sys.stdout.flush()

		except:
			print("Failed reading serial port")
			sys.stdout.flush()
			mChatting = False

	print("Serial Reader Thread Exited...")


def readBuffer():

	global mInBuffer
	global mLock

	if mLock == False:
		retBuffer = mInBuffer
		mInBuffer = []
		#print("Reading Data "+str(retBuffer))
		return retBuffer
	else:
		return []

def serialWritePacket(data):
	global packetOut

	if DEBUG == True:
		print("Writing data "+str(data))


	encodePacket(data, len(data))
	fillBuffer(packetOut)
	print("Writing data "+str(packetOut))
	sendBuffer()
	clearBuffer()
	packetOut = []


def serialReadPackets():
	packetList = []
	for packet in readBuffer():
		packetList.append(packet)
	return packetList


def fillBuffer(data):
	global mOutBuffer
	mOutBuffer = data
	if DEBUG == True:
		print("Filled Buffer = "+str(mOutBuffer))
	#sys.stdout.flush()


def sendBuffer():
	global mOutBuffer
	global mPort
	global mLock
	if DEBUG == True:
		print("Prepairing to write "+str(mOutBuffer))
	
	if mPort is not None:
		try:
			if waitForLock() == True:
				mOutBuffer.extend("\r")
				mLock = True
				mPort.write(mOutBuffer)
				mLock = False
			#if DEBUG = True:
			#print("Wrote Data "+str(mOutBuffer))
			#time.sleep(0.1)
		except:
			print("Could not write data "+str(mOutBuffer))
	else:
		print("Tried to write to nonexistent port")
	

	#print("Sending Buffer = "+str(mOutBuffer))
	#sys.stdout.flush()


def clearBuffer():
	global mOutBuffer
	mOutBuffer = []



def serialAvailable():
	global mPort

	bytesAvailable = 0
	try:
		if mPort.is_open:
			bytesAvailable = mPort.in_waiting
		else:
			print("Port Closed")
	except:
		bytesAvailable = 0

	return bytesAvailable


def serialPacketsAvailable():
	global mInBuffer
	return len(mInBuffer)


def serialIsConnectionOpen():
	return mChatting


def serialOpenConnection(portName, baudRate, timeout):

	global mPort
	global mChatting
	global mLock

	initPacket()
	mLock = False

	mPort = getSerialPort(portName, baudRate, timeout)

	if mPort is not None:
		mChatting = True
		Thread(target=serialReader, args=()).start()
		#Thread(target=serialWriter, args=()).start()
		print("Serial Connection opened!")
		return True
	else:
		mChatting = False
		print("Could not find serial port")
		return False

def isChatting():
	global mChatting

	return mChatting

def serialCloseConnection():

	global mChatting
	global mPort
	global mLock
	global mInBuffer
	global mOutBuffer
	mChatting = False
	if mPort.is_open:
		mPort.close()
	mPort		= None
	mLock 		= False
	mInBuffer 	= []
	mOutBuffer	= []
	print("Serial Connection Closed")


#Reset's the packetizer variables... Must be called everytime a packet is received
def initPacket():

    global mBytes
    global startFlag
    global escapeFlag
    global addToPacket
    global checksum
    global checksumIndex
    global packetIn

    #print("Initializing Packet")

    mBytes = 0
    startFlag = False
    escapeFlag = False
    addToPacket = False
    checksum = 0
    checksumIndex = 0
    packetIn = []

    #print("Packet Initialized")



#Handle each byte off the serial port
def handleIncomingByte(incomingByte):

	global checksum
	global packetIn
	global mInBuffer
	global mLock

	#print("Byte in = "+str(incomingByte))
	sys.stdout.flush()

	if decodePacket(incomingByte) == 1 and len(packetIn) > 1:
		check = ((~(int(checksum+1))) & int(255))
		check = 0
		#print("PACKET IN "+str(packetIn)+" Check = "+str(check))
		if check == 0:
			#print("Appending packet")
			mInBuffer.append(packetIn)
			#print("Prepairing to init packet")
			initPacket()
		else:
			print("Something is wrong check ="+str(check))





#Encodes packets to send over UART to Embedded processor
def encodePacket(data, count):

    global packetOut
    global packetIn
    global checksum
    global STX
    global ETX
    global DLE
    
    packetCount = 0
    tempByte = 0
    checksum = 0
    packetIndex = 2

    if count == 0:
        return packetOut
    

    #start condition
    packetOut.append(STX)
    packetOut.append(STX)

    #build packet
    for packetCount in range(0,count):

        if(data[packetCount] == DLE or data[packetCount] == ETX or data[packetCount] == STX):
            packetOut.append(DLE)
            packetIndex += 1
            
        packetOut.append(data[packetCount]);
        checksum += int(packetOut[packetIndex])
        packetIndex += 1
    


    #insert checksum
    tempByte = (((~(checksum))+1)&255)

    if tempByte == DLE :
        packetOut.append(DLE)
        packetIndex += 1
            

    packetOut.append(tempByte)
    packetIndex += 1

    #stop condition
    packetOut.append(ETX)
    packetIndex += 1



#Decodes data coming in over UART byte by byte
def decodePacket(b):

    global packetOut
    global packetIn
    global startFlag
    global escapeFlag
    global checksum
    global mBytes
    global addToPacket
    global STX
    global ETX
    global DLE

    #Check for Escape sequence
    if b == DLE and not escapeFlag :
        escapeFlag = True
        startFlag = False
    

    #Find first start sequence
    elif b == STX and not escapeFlag and not startFlag :
        startFlag = True
    

    #Find second start sequence
    elif b == STX and not escapeFlag and startFlag :
        addToPacket = True
        startFlag = False
    

    #Check for end sequence
    elif b == ETX and not escapeFlag :
        startFlag = False
        return 1
    

    #Check for valid byte
    elif(addToPacket):
        startFlag = False
        escapeFlag = False
        packetIn.append(b)
        checksum += b
        mBytes += 1
    
    else:
        if DEBUG:
            print("Byte: "+str(b)+" not handled")
    
    
    return -1;

if __name__ == '__main__':
	serialOpenConnection("/dev/ttyO1", 115200, 0)