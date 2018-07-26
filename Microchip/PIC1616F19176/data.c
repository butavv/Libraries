
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
#include "peripherals.h"

static messageQ_t messageQ[MESSAGE_Q_MAX_SIZE];
static UINT8 messageQ_size = 0;

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

static results_t results;

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
    txMessage.header.messageSrc = UART_PIC16;
    
    static UINT8 packet[ PACKETSIZE_MAX ];      //Allocate RAM space
    UINT16 packetSize = 0;
    if(nBytes < PACKETSIZE_MAX){
        packetSize = encodePacket(&txMessage, nBytes, packet, PACKETSIZE_MAX);
    }

    if(packetSize > 0)
    {
        SendUART(packet, packetSize+1);
        
        if(txMessage.header.messageId == MSG_TEST){
            results.nSent++;
        }
    }
}

//Converts a 4 byte array to a UINT32 
UINT32 byteArrToUint32(UINT8 *byteArr)
{
    UINT8 i;
    UINT32 retval = 0;
    for(i=0; i<4; i++)
    {
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
        message_t txMessage;
        
        
        
        //Get Header
        rxMessage.header.nBytes = packet[0];
        rxMessage.header.messageId = packet[1];
        rxMessage.header.messageSrc = packet[2];
        rxMessage.header.messageDst = packet[3];
        
        if(rxMessage.header.messageDst == UART_PIC16){
            
            for(i=HEADER_SIZE; i<nBytes; i++)
            {
                rxMessage.dat[i] = packet[i];
            }


            switch( rxMessage.header.messageId )
            {
                case MSG_TEST:
                    
                    //update stats
                    results.nRcvd++;
                    
                    /*if(packetStats.nRcvd > 255)
                    {
                        packetStats.nRcvd = 0;
                        CLD_SOL ^= 1;
                    }*/
                    if(rxMessage.header.messageSrc == UART_CPU)
                    {
                        //Echo packet
                        message_t txMessage = rxMessage;
                        txMessage.header.messageDst = UART_CPU;
                        sendPacket(txMessage);
                    }
                    
                    break;
                case MSG_RESULTS:
                    //Do nothing
                    break;
                case MSG_GET_RESULTS:
                    //Send results packet
                    txMessage.header = rxMessage.header;
                    txMessage.header.nBytes = HEADER_SIZE + 6;
                    txMessage.header.messageId = MSG_RESULTS;
                    txMessage.header.messageDst = UART_CPU;
                    txMessage.dat[HEADER_SIZE    ] = (UINT8)(results.nSent >> 8);
                    txMessage.dat[HEADER_SIZE + 1] = (UINT8)(results.nSent);
                    txMessage.dat[HEADER_SIZE + 2] = (UINT8)(results.nRcvd >> 8);
                    txMessage.dat[HEADER_SIZE + 3] = (UINT8)(results.nRcvd);
                    
                    //INT16 ntc_1 = ReadNTC1();
                    INT16 ntc_3 = ReadNTC3();
                    
                    txMessage.dat[HEADER_SIZE+4] = (UINT8)(ntc_3 >> 8);
                    txMessage.dat[HEADER_SIZE+5] = (UINT8)(ntc_3);
                    
                    sendPacket(txMessage);
                    break;
            }
        }
        
    }
    
    
}



void scheduleMessage(message_t msg, UINT32 delay, UINT8 index)
{

    messageQ_size;
    if( messageQ_size < MESSAGE_Q_MAX_SIZE)
    {
        messageQ[index].msg = msg;
        messageQ[index].sendTime = GetUptime() + delay;
        messageQ_size++;
    }
    
}

void InitializeDataHandler()
{
    initDataPacket(&UART.rcv);
    UART.enabled = TRUE;
    UINT8 i;
    
    /*for(i=0; i<MESSAGE_Q_MAX_SIZE; i++)
    {
        message_t msg;
        msg.header.messageId = MSG_TEST;
        msg.header.nBytes = PACKETSIZE_STANDARD;
        msg.header.messageDst = UART_PIC32;
        UINT8 j;
        for(j=HEADER_SIZE; j<PACKETSIZE_STANDARD; j++)
        {
            SeedLFSR(GetUptime()+j, GetUptime()+i);
            msg.dat[j] = (UINT8)GetRandom();
        }
        scheduleMessage(msg, MSG_INTERVAL*i, i);
    }*/
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
    /*for(i=0; i<messageQ_size; i++){
        //messageQ_t msgNode = messageQ[i];
        if( GetUptime() >= messageQ[i].sendTime)
        {
            sendPacket(messageQ[i].msg); 
            messageQ_size--;
            
            //Schedule new message
            message_t msg;
            msg.header.messageId = MSG_TEST;
            msg.header.nBytes = PACKETSIZE_STANDARD;
            msg.header.messageDst = UART_PIC32;
            UINT8 j;
            for(j=HEADER_SIZE; j<PACKETSIZE_STANDARD; j++)
            {
                SeedLFSR(GetUptime()+j, GetUptime()+i);
                msg.dat[j] = (UINT8)GetRandom();
            }
            scheduleMessage(msg, MSG_INTERVAL, i);
        }
    }*/
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