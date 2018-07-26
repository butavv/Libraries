import serialUtil as ser
import sys
from threading import Thread
import time
import random
import converter

global mPacketsReceived 
global mPacketsSent

mPacketsSent = 0
mPacketsReceived = 0

MSG_TEST = 0
MSG_RESULTS = 1
MSG_GET_RESULTS = 2

UART_CPU = 0
UART_PIC32 = 1
UART_PIC16 = 2

HEADER_SIZE = 4

MAX_DATA_LEN = 20

global UART_ID 
UART_ID = 0

def handlePacket(packet):
	global mPacketsReceived
	#print("Packet In")
	#sys.stdout.flush()
	mPacketsReceived += 1


def pullEmbedded(timeout):
	#print("Starting pull thread")
	sys.stdout.flush()
	count = 0
	while count < timeout:
		start = time.time()
		packetsIn = ser.serialReadPackets()
		if len(packetsIn) > 0:
			for packet in packetsIn:
				print("Packet in = "+str(packet))
				if(packet[3] == UART_ID):
					end = time.time()
					count += (end-start)
					print("Packet received after = "+str(count*1000)+" ms")
					sys.stdout.flush()
					return (1, packet, count)
				else:
					print("ID doesn't match "+str(packet[3]))
					sys.stdout.flush()

		end = time.time()

		#Minimum sleep is 10ms  
		#time.sleep(0.01)
		count += (end-start)


	if(count == timeout):
		print("Packet NOT received after = "+str(count*1000)+" ms")
		sys.stdout.flush()
		return (-1, None, count)
	else:
		print("Packet NOT received after = "+str(count*1000)+" ms")
		sys.stdout.flush()
		return (0, None, count)



def addHeader(data, messageType, target):
	global UART_ID
	retPacket = [ len(data)+4, messageType, UART_ID, target ]
	retPacket.extend(data)
	return retPacket


def comTest(samples=1000, timeout=0.1, target=UART_PIC16):

	##print("Enter timeout in seconds")
	#sys.stdout.flush()
	#timeout = raw_input()

	print("\nEnter number of samples")
	sys.stdout.flush()
	samples = raw_input()


	print("\nEnter data length (1-"+str(MAX_DATA_LEN)+"), other for random")
	sys.stdout.flush()
	dataLen = raw_input()
	

	print("\n\nPrepairing data for COM test...")
	sys.stdout.flush()

	total = int(samples)
	count = 0
	packetsOut = []
	packetsIn = []
	packetsReceived = 0
	packetsSent = 0
	timeElapsed = 0

	for val in range(0,total):
		packetsOut.append(addHeader(genData(int(dataLen)), MSG_TEST, target))

	print("COM test in progress...")
	sys.stdout.flush()
	for packetOut in packetsOut:
		packetsSent += 1
		ser.serialWritePacket( packetOut )
		print("Sent Packet")
		sys.stdout.flush()
		packetsIn.append(pullEmbedded(timeout))
		count += 1
		#print("Count = "+str(count))

	print("\n\nCOM test finished... prepairing results\n\n")
	
	sys.stdout.flush()

	i=0
	errorLengthsSum = 0
	errorLengthsCount = 0
	lengthsSum = 0
	lengthsCount = 0
	corruptPackets = 0
	for packetIn in packetsIn:
		timeElapsed += (packetIn[2]*1000)
		if( packetIn[0] == 1):
			print("\nPacket "+str(i))
			sys.stdout.flush()
			packetsReceived += 1
			lengthsSum += len(packetIn[1])
			lengthsCount += 1
			j=0
			for byte in packetsOut[i]:
				if j>3:
					print("DATA Byte In = "+str(packetIn[1][j])+" Byte Out = "+str(byte))
					if byte != packetIn[1][j]:
						print("CORRUPT")
						sys.stdout.flush()
						corruptPackets += 1
						break
				else:
					print("HEADER: Byte In = "+str(packetIn[1][j])+" Byte Out = "+str(byte))
				j += 1
		else:
			print("Packet "+str(i)+" never received")
			sys.stdout.flush()
			try:
				errorLengthsSum += len(packetsOut[i])
				errorLengthsCount += 1
			except:
				print(str(i)+" "+str(errorLengthsCount))
		i += 1

	print("\n\n**************** COM TEST RESULTS *************************")
	print(" Packets Sent = "+str(packetsSent))
	print(" Packets Received = "+str(packetsReceived))
	print(" Corrupted Packets = "+str(corruptPackets))
	if(lengthsCount > 0):
		print(" Average Success Packet Length = "+str(lengthsSum/lengthsCount))
	
	if( errorLengthsCount > 0):
		print(" Average Error Packet Length = "+str(errorLengthsSum/errorLengthsCount))
	print(" Error = "+str( 100 - ((float(packetsReceived-corruptPackets)/float(packetsSent))*100.0)) +"%" )
	print(" Time Elapsed = "+str(timeElapsed)+" ms, "+str((timeElapsed/1000))+" seconds")
	print("\n\n Press Enter to Continue")
	sys.stdout.flush()

	raw_input()

def genData(dataLen):
	testData = []
	if dataLen not in range(1,MAX_DATA_LEN):
		dataLen = random.randrange(1, MAX_DATA_LEN, 1)
	for val in range(0, dataLen):
		rand = random.randrange(0, 255, 1)
		testData.append(rand)

	return testData


if __name__ == '__main__':

	global mPacketsReceived 
	global mPacketsSent

	if len(sys.argv) < 2:
		print("Error: Must enter COM port")
		sys.exit(0)

	
	try:
		ser.serialOpenConnection(sys.argv[1], 115200, 0)
	except:
		print("Error: Failed opening serial port")
		sys.stdout.flush()
		sys.exit(0)

	#Thread(target=pullEmbedded, args=()).start()

	print("\nStarting main loop...")
	sys.stdout.flush()

	packetsReceived = 0
	packetsSent = 0
	timeout = 0.1

	while(ser.isChatting):
		
		print("\n\nType \"send16\" or \"send32\" to send 1 packet. \"test16\" or \"test32\" to run com test. \"to\" to set timeout. \"r\" to get test results\n")
		sys.stdout.flush()
		command = raw_input()
		

		if command == "send16":
			print("\nEnter data length (1-"+str(MAX_DATA_LEN)+"), other for random")
			sys.stdout.flush()
			dataLen = raw_input()
			packetOut = []
			packetOut = addHeader(genData(int(dataLen)), MSG_TEST, UART_PIC16)
			ser.serialWritePacket( packetOut )
			packetsSent += 1
			
			result = pullEmbedded(timeout)
			if( result[0] == 1):
				i=0
				packetsReceived += 1
				for byte in packetOut:
					if i>3:
						print(str(i)+". DATA Byte Out = "+str(byte)+"\t:\t  Byte In = "+str(result[1][i]))
						if byte != result[1][i]:
							packetsReceived -= 1
							break
					else:
						print(str(i)+". HEADER Byte Out = "+str(byte)+"\t:\t  Byte In = "+str(result[1][i]))

					i += 1
			print("Total Packets Sent = "+str(packetsSent))
			print("Total Packets Received = "+str(packetsReceived))
			sys.stdout.flush()

		elif command == "send32":
			print("\nEnter data length (1-"+str(MAX_DATA_LEN)+"), other for random")
			sys.stdout.flush()
			dataLen = raw_input()
			packetOut = []
			packetOut = addHeader(genData(int(dataLen)), MSG_TEST, UART_PIC32)
			ser.serialWritePacket( packetOut )
			packetsSent += 1
			
			result = pullEmbedded(timeout)
			if( result[0] == 1):
				i=0
				packetsReceived += 1
				for byte in packetOut:
					if i>3:
						print(str(i)+". DATA Byte Out = "+str(byte)+"\t:\t  Byte In = "+str(result[1][i]))
						if byte != result[1][i]:
							packetsReceived -= 1
							break
					else:
						print(str(i)+". HEADER Byte Out = "+str(byte)+"\t:\t  Byte In = "+str(result[1][i]))

					i += 1
			print("Total Packets Sent = "+str(packetsSent))
			print("Total Packets Received = "+str(packetsReceived))
			sys.stdout.flush()

		elif command == "test16":
			comTest(timeout=timeout, target=UART_PIC16)

		elif command == "test32":
			comTest(timeout=timeout, target=UART_PIC32)

		elif command == "to":
			print("\nEnter timeout in ms")
			sys.stdout.flush()
			to = raw_input()
			timeout = float(to)/1000

		elif command == "r":
			packetOut = addHeader([], MSG_GET_RESULTS, UART_PIC16)
			ser.serialWritePacket(packetOut)
			result = pullEmbedded(timeout)
			#print("\nRESULT = "+str(result))
			if(result[1] is not None):
				print('\n\n\n*****RESULTS*****')
				print("MCU Packets Sent = "+str(converter.byteArrayToShort(result[1], HEADER_SIZE, True)))
				print("MCU Packets Received = "+str(converter.byteArrayToShort(result[1], HEADER_SIZE+2, True)))
				print("NTC3 = "+str(converter.byteArrayToShort(result[1], HEADER_SIZE+4, True)))

		else:
			print("\n\nInvalid Command")

		#raw_input("Press Enter to start COM test...\n")
		##sys.stdout.flush()
		#Thread(target=comTest, args=()).start()
	print("Communication Terminated")
	sys.stdout.flush()