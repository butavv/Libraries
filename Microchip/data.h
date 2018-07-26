/* 
 * File:   data.h
 * Author: Nick
 *
 * Created on April 26, 2018, 1:56 PM
 */

#ifndef DATA_H
#define	DATA_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PACKETSIZE_MAX                80        //Max size of incoming packet, bytes
#define PACKETSIZE_STANDARD           6 
#define STX 0x0F
#define ETX 0x04
#define DLE 0x05

#define TX_MESSAGE_PERIOD           100
#define HEADER_SIZE                 4    

#define BROADCAST_SN                0xFFFFFFFF
#define APP_SN                      0
#define UART_ID                     2
    
#define MSG_TEST					0
#define MSG_RESULTS					1
#define MSG_GET_RESULTS				2
    
#define UART_CPU                    0    
#define UART_PIC32                  1
#define UART_PIC16                  2
    
typedef enum
{
    UART_RDY,
    UART_RCV
}UARTstate_t;


typedef struct
{
	unsigned char Start;
	unsigned long Checksum;
	unsigned char State;
	unsigned int dataIndex;            //Next byte location; also size of packet
	unsigned char data[PACKETSIZE_MAX];    //Raw data
}dataPacket_t;


typedef struct
{
 	UINT8 nBytes;		//Total number of bytes in this message, including header
 	UINT8 messageId;	//Message ID ? what type of message is it (see enumeration)
    UINT8 messageSrc;   //Sender ID
    UINT8 messageDst;   //Recipient ID
} header_t;

typedef union
{
	struct{
        header_t header;
    };
    UINT8 dat[PACKETSIZE_MAX];
}message_t;



#ifdef	__cplusplus
}
#endif

// returns the size of the encoded packet
// unsigned char *data points to the data that is to be encoded
// unsigned char count is the number of bytes that *data points to
// unsigned char *packet points to a place in the client code to put the encoded
// packet.  Ensure that you have enough RAM allocated.
int encodePacket(unsigned char *data, unsigned char count,
        unsigned char *packet, unsigned int maxPacket );

 // called each time you create a new dataPacket
void initDataPacket(dataPacket_t *packet);

// This function is fed serially, one byte at a time, until a valid packet is 
// decoded. Returns zero if a valid packet is not found yet.  If a valid packet
// is decoded it returns the number of decoded data bytes in dataPacket.data.
// unsigned char theByte is the byte to be decoded
//dataPacket *packet is the state machine data that the client program allocated
int decodePacket(unsigned char theByte, dataPacket_t *packet);


void sendPacket( message_t txMessage );
void receivePacket(UINT8 *packet, UINT8 nBytes);

UINT32 byteArrToUint32(UINT8 *byteArr);
void scheduleMessage(message_t msg, UINT32 delay);

extern void InitializeDataHandler();
extern void HandleData();

#endif	/* DATA_H */

