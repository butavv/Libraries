
//**********************************************************************
//
//  Smart Vision Lights
//  
//  Compiler: XC8 v1.45
//
//  File: data.c
//
//  Description:
//  This is the data processor for the LIN communication line  
//
//  2017 Phase 1 Engineering, LLC
//
//**********************************************************************
#include "hardware.h"
#include "types.h"
#include "configuration.h"
#include "uart.h"
#include "data.h"
#include "timers.h"
#include "random.h"

static BOOL mMsgInQueue = FALSE;
static UINT64 mSendTime = 0;
static message_t mTxMessage;

//UART Data Structure definition
static struct
{
    BOOL enabled;
    dataPacket_t rcv;
    UINT8 nRcv;
    UINT8 nExp;
    UARTstate_t state;
    UINT64 lastMessageTime; 
}UART;


//Function: sendPacket
//Inputs:   data = pointer data to send over UART.  n = number of bytes to send
//Outputs:  none
//
//Description:
//Sends a data packet out over UART 
void sendPacket(message_t txMessage)
{
	UINT8 nBytes;
	
    nBytes = txMessage.header.nBytes;
    txMessage.header.messageSrc = UART_PIC32;
    static UINT8 packet[ PACKETSIZE_MAX ];      //Allocate RAM space
    UINT16 packetSize = 0;
    if(nBytes < PACKETSIZE_MAX){
        packetSize = encodePacket(&txMessage, nBytes, packet, PACKETSIZE_MAX);
    }

    if(packetSize > 0){
        SendUART(packet, packetSize+1);
    }
    
}

//Converts a 4 byte array to a UINT32 
UINT32 byteArrToUint32(UINT8 *byteArr)
{
    UINT8 i;
    UINT32 retval = 0;
    for(i=0; i<4; i++){
        retval |= ( (UINT32)byteArr[i] << (i*8) );
    }
    return retval;
}

//Handler for an incoming packet over the LIN bus
void receivePacket(UINT8 *packet, UINT8 nBytes)
{
    if(nBytes >= HEADER_SIZE){
        
        //Fill in data from packet
        UINT8 i;
        message_t rxMessage;
        
        //Get Header
        rxMessage.header.nBytes = packet[0];
        rxMessage.header.messageId = packet[1];
        rxMessage.header.messageSrc = packet[2];
        rxMessage.header.messageDst = packet[3];
        
        if(rxMessage.header.messageDst == UART_PIC32){
            for(i=HEADER_SIZE; i<nBytes; i++)
            {
                rxMessage.dat[i] = packet[i];
            }

            //Echo packet
            message_t txMessage = rxMessage;
            txMessage.header.messageDst = rxMessage.header.messageSrc;
            sendPacket(rxMessage);
        }
    }
    
    
}



void scheduleMessage(message_t msg, UINT32 delay)
{
    mTxMessage = msg;
    mSendTime = GetUptime() + delay;
    mMsgInQueue = TRUE;
}

void InitializeDataHandler()
{
    initDataPacket(&UART.rcv);
    UART.enabled = TRUE;
}

//Description:
//Get inputs from UART and decodes packets from raw data.
//Call in main loop repeatedly.
void HandleData()
{
    UINT8 i;
    UINT16 temp16;
    unsigned char data[] = "a";       //Must use the "" to get a null character
    unsigned char incomingByte;
    
    //Get Data coming in
    if( (GetNbytesReadyUART() > 0 ))
    {
        ReadUART(data, 1);
        incomingByte = data[0];
        switch(UART.state)
        {
            case UART_RDY:
                if( decodePacket(incomingByte, &UART.rcv) > 0){
                    UART.state = UART_RCV;       //Recieved a full packet.. Wait for /r
                }
                break;
            case UART_RCV:
                receivePacket(UART.rcv.data, UART.rcv.dataIndex);
                UART.state = UART_RDY;
                initDataPacket(&UART.rcv);
                break;
        }
    }
    
    //Handle any messages that are scheduled to be sent
    if( mMsgInQueue)
    {
        if( GetUptime() >= mSendTime)
        {
            sendPacket(mTxMessage);
        }
    }
}

int encodePacket(unsigned char *data, unsigned char count,
        unsigned char *packet, unsigned int maxPacket )
{			
	unsigned int PacketCount;
	unsigned char tempByte;
	unsigned long Checksum = 0;
	unsigned int packetIndex = 2;
	
	if( count == 0 )
		return 0;
	
	//start condition
	packet[0] = STX;
	packet[1] = STX;

	//build the packet data and stuff byte if necessary
	for(PacketCount = 0; PacketCount < count; PacketCount++)
	{
		switch (data[PacketCount])
		{
			case STX:
			case ETX:
			case DLE:
			{
				packet[packetIndex] = DLE;
				packetIndex++;
				if( packetIndex == maxPacket )
					return 0;
				break;
			}
		}
		
		packet[packetIndex] = data[PacketCount]; 
		Checksum = Checksum + (unsigned long)packet[packetIndex];
		packetIndex++;
		if( packetIndex == maxPacket )
			return 0;
	}

	//insert the checksum
	tempByte =  (unsigned char)(((~(Checksum)) + 1) & 255);
	switch (tempByte)
	{
		case STX:
		case ETX:
		case DLE:
		{
			packet[packetIndex] = DLE;
			packetIndex++;
			if( packetIndex == maxPacket )
				return 0;
			break;
		}
	}
	
	packet[packetIndex] =  tempByte;
	packetIndex++;
	if( packetIndex == maxPacket )
		return 0;

	//insert the stop condition
	packet[packetIndex] = ETX;
	packetIndex++;
	
	return packetIndex;
}

void initDataPacket(dataPacket_t *packet)
{
	packet->State = 0;
	return;
}

int decodePacket(unsigned char theByte, dataPacket_t *packet)
{
	if( packet->State == 0 )
		goto State0;

	if( packet->State == 1)
		goto State1;
	
	if( packet->State == 2)
		goto State2;

	if( packet->State == 3)
		goto State3;
		
	packet->State = 0;
	return 0;

State0:
	packet->Start = 2;
	packet->State = 1;
	
	//Scan for a start condition
State1:
	if ( packet->Start )
	{				
		if (theByte == STX)
                {
                    packet->Start--;
                }
		else 
			packet->Start = 2;

		return(0);
	}

	// Get the data and unstuff when necessary
	packet->Checksum = 0;
	packet->dataIndex = 0;
	packet->State = 2;
State2:		
	while (theByte != ETX)
	{	
		switch (theByte)
		{
			case STX: 
			{
				goto State0;
			}

			case DLE: 
			{
				packet->State = 3;
				return 0;
			}
			
			default:
			{
State3:
				if (packet->dataIndex < PACKETSIZE_MAX) 
				{
					packet->data[packet->dataIndex] = theByte;
					packet->dataIndex++;
				}
				
				packet->Checksum = packet->Checksum + (unsigned int)theByte;
				packet->State = 2;
				return 0;
			}
		}
	}	
	packet->State = 0;

	//Test the checksum
	if( (unsigned char)(((~(packet->Checksum)) + 1) & 255) )
        {
            return 0;
        }
	return (packet->dataIndex);
}