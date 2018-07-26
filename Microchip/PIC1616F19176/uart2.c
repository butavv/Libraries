

//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: uart2.c
//
//  Description:
//  This file contains two UART drivers.  Hardware resources used: UART1, UART2
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#include "configuration.h"
#include "hardware.h"
#include "types.h"
#include "uart2.h"

//---------------------------- Local Definitions -------------------------------
typedef struct              //UART receive buffer
{
    char buf[UART2_BUF_RCV]; //Buffer string
    UINT8 head;             //Head of buffer
    UINT8 tail;             //Tail of buffer
    UINT8 nB;               //Number of bytes left in buffer
} uartBufr2_t;
typedef struct              //UART transmit buffer
{
    char buf[UART2_BUF_SEND]; //Buffer string
    UINT8 head;             //Head of buffer
    UINT8 tail;             //Tail of buffer
    UINT8 nB;               //Number of bytes left in buffer
} uartBufs2_t;

//---------------------------- Static Variables --------------------------------
static uartBufr2_t rcv2;       //Buffer for receiving UART data
static uartBufs2_t send2;      //Buffer for sending UART data

//-------------------------- Function Definitions ------------------------------

//Function: InitializeUART
//Inputs: none
//Outputs: none
//
//Description:
//Set up for UART operations, in and out of the UART2 port
void InitializeUART2( void )
{   //Set baud rate 115200: 64MHz/(4 * (138+1) = 115,107 (-0.08%)
    TX2STAbits.BRGH = 1;                //1: High-speed mode (divide by 4)
    BAUD2CONbits.BRG16 = 1;             //1: 16-bit baud rate generator
    BAUD2CONbits.ABDEN = 0;             //0: No auto-baud
    BAUD2CONbits.SCKP = 0;              //0: Idle high
    SP2BRGH = 0;                        //0: high byte = 0
    SP2BRGL = 138;                       //138: divide by 138+1
    BAUD2CONbits.SCKP = 0;             //0: Idle TX state is high

    //Initialize receive buffer
    rcv2.head = 0;
    rcv2.tail = 0;
    rcv2.nB = 0;
    
    //Initialize UART transmit buffer
    send2.head = 0;
    send2.tail = 0;
    send2.nB = 0;
    
    //Configure UART
    RC2STAbits.RX9 = 0;                 //0: 8-bit reception
    RC2STAbits.ADDEN = 0;               //0: Disable address detection
    TX2STAbits.SYNC = 0;                //0: Asynchronous
    TX2STAbits.TX9 = 0;                 //0: 8-bit data mode
    RC2STAbits.FERR = 0;                //0: Clear framing error
    RC2STAbits.OERR = 0;                //0: Clear overrun error
    //IPR3bits.RC2IP = 0;                 //0: Low interrupt priority (RX)
    //IPR3bits.TX2IP = 0;                 //0: Low interrupt priority (TX)
    PIE3bits.RC2IE = 1;                 //1: Enable interrupts
    
    //Pin Peripheral selection
    RX2PPS = 0b00011001; //D1
    RD0PPS = 0xE; //EUSART2TX

    //Turn on the UART
    RC2STAbits.CREN = 1;                //1: Enable receiver
    TX2STAbits.TXEN = 1;                //1: Enable transmitter
    RC2STAbits.SPEN = 1;
    //Enable transmit complete interrupt after a byte was sent, not here
    //PIE1bits.TX1IE = 1;

    

}

//Function: HandleDebugUARTint
//Inputs: none
//Outputs: none
//
//Description:
//Handles debug UART TX/RX data interrupt (low priority)
void HandleUART2int( void )
{
    //Check UART transmit
    if (1==TX2IF)
    {
        TX2IF = 0;                   //Clear transmit interrupt flag
        if (1==PIE3bits.TX2IE)
        {
            //Finished transmitting. Is there another byte to send?
            if (send2.nB > 0)
            {
                //Send next byte to buffer
                TX2REG = send2.buf[send2.tail++];
                if (send2.tail >= UART2_BUF_SEND)
                {
                    send2.tail = 0;
                }
                //Update buffer count
                send2.nB--;
            }
            else
            {
                PIE3bits.TX2IE = 0; //No data left to send: disable interrupt
            }
        }
    }

    //Check UART receive
    if (1==RC2IF)
    {
        RC2IF = 0;                          //Clear receive interrupt flag
        if (1==PIE3bits.RC2IE)
        {
            //Received byte via UART
            //Save into buffer
            rcv2.buf[rcv2.head++] = RC2REG;   //Save byte from UART into buffer
            if (rcv2.head >= UART2_BUF_RCV)
            {
                rcv2.head = 0;
            }
            if (rcv2.nB < UART2_BUF_RCV)
            {
                rcv2.nB++;                   //Update buffer count
            }
        }
    }
}

//Function: StartXmit
//Inputs: none
//Outputs: none
//
//Description:
//Starts transmitting UART data if not already in process
//NOTE: This function includes an interrupt CRITICAL SECTION.
static void StartXmit2( void )
{
    //Check if there is no transmission in process
    if (0==PIE3bits.TX2IE)
    {
        //Send next byte from buffer to UART peripheral
        TX2REG = send2.buf[send2.tail++];
        if (send2.tail >= UART2_BUF_SEND)
        {
            send2.tail = 0;
        }
        send2.nB--;                  //Update buffer count
        PIE3bits.TX2IE = 1;         //Enable transmission interrupt
    }
    else
    {
        //If already transmitting, don't need to do anything
    }

}          //Start transmission

//Function: SendUART
//Inputs:
//  *str:   String of characters to send - this points to a RAM buffer location
//  nBytes: Number of bytes to be sent
//Outputs:  none
//
//Description:
//Sends a string of data out via UART by saving bytes into a buffer, max size
//UART_BUF, and initiating transmission. Sends as many bytes as there is room in
//the buffer and returns number of bytes that were not sent.
UINT8 SendUART2( char *str, UINT8 nBytes )
{
    //Save current buffer size so it doesn't change in an interrupt
    //Number of bytes available in buffer
    INT16 room = UART2_BUF_SEND - (INT16)send2.nB;
    UINT16 bytesLeft = 0;
    UINT16 i, gieh;

    if (room < 0)           //Not enough room in buffer?
    {
        nBytes += room;     //Don't add more than there is room for
        bytesLeft = 0-room;
    }

    for (i=0; i<nBytes; i++)
    {
        send2.buf[send2.head++] = str[i];
        if (send2.head >= UART2_BUF_SEND)
        {
            send2.head = 0;
        }
    }

    GIE = 0;               //CRITICAL SECTION: DISABLE INTERRUPTS
    send2.nB += nBytes;      //Update buffer count
    GIE = 1;               //CRITICAL SECTION: DISABLE INTERRUPTS

    StartXmit2();            //Start transmission

    return bytesLeft;
}

//Function: SendUARTcnst
//Inputs:
//  *str:   String of characters to send - this points to a CONST (ROM) location
//  nBytes: Number of bytes to be sent
//Outputs:  none
//
//Description:
//Sends a string of data out via UART by saving bytes into a buffer, max size
//UART_BUF, and initiating transmission. Sends as many bytes as there is room in
//the buffer and returns number of bytes that were not sent.
UINT8 SendUART2cnst( const char *str, UINT8 nBytes )
{
    //Save current buffer size so it doesn't change in an interrupt
    //Number of bytes available in buffer
    INT16 room = UART2_BUF_SEND - (INT16)send2.nB;
    UINT16 bytesLeft = 0;
    UINT16 i, gieh;

    if (room < 0)           //Not enough room in buffer?
    {
        nBytes += room;     //Don't add more than there is room for
        bytesLeft = 0-room;
    }
    for (i=0; i<nBytes; i++)
    {
        send2.buf[send2.head++] = str[i];
        if (send2.head >= UART2_BUF_SEND)
        {
            send2.head = 0;
        }
    }
    //gieh = GIEH;
    GIE = 0;               //CRITICAL SECTION: DISABLE INTERRUPTS
    send2.nB += nBytes;      //Update buffer count
    GIE = 1;               //CRITICAL SECTION: DISABLE INTERRUPTS

    StartXmit2();            //Start transmission

    return bytesLeft;
}

//Function: GetNbytesReadyUART
//Inputs: none
//Outputs: unsigned char n - number of bytes in buffer
//
//Description:
//Returns the number of bytes currently available in the UART receive buffer.
UINT8 GetNbytesReadyUART2( void )
{
    return rcv2.nB;
}

//Function: ReadUART
//Inputs: char *str - pointer to data to send; UINT8 nBytes - N bytes to send
//Outputs:
//  BOOL results - TRUE means it read the requested number, FALSE means not
//  enough data was available to read
//
//Description:
//Reads the designated number of bytes out from the receive UART buffer.
BOOL ReadUART2( char *str, UINT8 nBytes )
{
    BOOL retVal = FALSE;
    UINT8 gieh;

    if (rcv2.nB>0)
    {
        if (rcv2.nB < nBytes)
        {
            nBytes = rcv2.nB;
        }
        else
        {
            retVal = TRUE;
        }

        UINT8 i;
        for (i=0; i<nBytes; i++)
        {
            str[i] = rcv2.buf[rcv2.tail++];
            if (rcv2.tail >= UART2_BUF_RCV)
            {
                rcv2.tail = 0;
            }
        }
        gieh = GIE;
        GIE = 0;               //CRITICAL SECTION: DISABLE INTERRUPTS
        rcv2.nB -= nBytes;       //Update buffer count
        GIE = gieh;            //CRITICAL SECTION: DISABLE INTERRUPTS
    }

    return retVal;
}
