
//**********************************************************************
//
//  SmartVision Lights Control Firmware
//
//  Compiler: XC8 v1.44
//
//  File: uart.c
//
//  Description:
//  This file contains the UART driver.  Hardware resources used: UART1
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#include "hardware.h"
#include "types.h"
#include "uart.h"

//---------------------------- Local Definitions -------------------------------
typedef struct              //UART receive buffer
{
    char buf[UART_BUF_RCV]; //Buffer string
    UINT8 head;             //Head of buffer
    UINT8 tail;             //Tail of buffer
    UINT8 nB;               //Number of bytes left in buffer
} uartBufr_t;
typedef struct              //UART transmit buffer
{
    char buf[UART_BUF_SEND]; //Buffer string
    UINT8 head;             //Head of buffer
    UINT8 tail;             //Tail of buffer
    UINT8 nB;               //Number of bytes left in buffer
} uartBufs_t;

//---------------------------- Static Variables --------------------------------
static uartBufr_t rcv;       //Buffer for receiving UART data
static uartBufs_t send;      //Buffer for sending UART data

//-------------------------- Function Definitions ------------------------------

//Function: InitializeUART
//Inputs: none
//Outputs: none
//
//Description:
//Set up for UART operations, in and out of the UART1 port
void InitializeUART( void )
{   //Set baud rate 9600: 32MHz/(4 * (833+1)) = 9592.33 (-0.08%)
    TXSTAbits.BRGH = 1;                //1: High-speed mode (divide by 4)
    BAUDCONbits.BRG16 = 1;             //1: 16-bit baud rate generator
    BAUDCONbits.ABDEN = 0;             //0: No auto-baud
    BAUDCONbits.SCKP = 0;              //0: Idle high
    //SPBRGH = 0;
    //SPBRGL = 207;
    SPBRGH = 0b00000011;               //0: high byte = 0
    SPBRGL = 0b01000001;               //833: divide by 833+1
    //BAUDCONbits.CKTXP = 0;           //0: Idle TX state is high

    //Initialize receive buffer
    rcv.head = 0;
    rcv.tail = 0;
    rcv.nB = 0;

    //Initialize UART transmit buffer
    send.head = 0;
    send.tail = 0;
    send.nB = 0;

    //Configure UART
    RCSTAbits.RX9 = 0;                 //0: 8-bit reception
    RCSTAbits.ADDEN = 0;               //0: Disable address detection
    
    TXSTAbits.SYNC = 0;                //0: Asynchronous
    TXSTAbits.TX9 = 0;                 //0: 8-bit data mode
    TXSTAbits.TXEN = 1;                 //Transmit enabled
    
    RCSTAbits.FERR = 0;                //0: Clear framing error
    RCSTAbits.OERR = 0;                //0: Clear overrun error
    //IPRbits.RC1IP = 1;                 //1: High interrupt priority (RX)
    //IPRbits.TX1IP = 1;                 //1: High interrupt priority (TX)
    PIE1bits.RCIE = 1;                 //1: Enable RX interrupts
    PIE1bits.TXIE = 1;                  //1: Enable TX interrupts
    
    //Pin Peripheral selection
    APFCON0bits.RXDTSEL = 1;            //RX function on RA1
    APFCON0bits.TXCKSEL = 1;            //TX function on RA0

    //Turn on the UART
    RCSTAbits.CREN = 1;                //1: Enable receiver
    TXSTAbits.TXEN = 1;                //1: Enable transmitter
    RCSTAbits.SPEN = 1;
    
}

//Function: HandleUARTint
//Inputs: none
//Outputs: none
//
//Description:
//Handles debug UART TX/RX data interrupt (low priority)
void HandleUARTint( void )
{
    //Check UART transmit
    if (1==TXIF)
    {
        TXIF = 0;                   //Clear transmit interrupt flag
        if (1==PIE1bits.TXIE)
        {
            //Finished transmitting. Is there another byte to send?
            if (send.nB > 0)
            {
                //Send next byte to buffer
                TXREG = send.buf[send.tail++];
                if (send.tail >= UART_BUF_SEND)
                {
                    send.tail = 0;
                }
                //Update buffer count
                send.nB--;
            }
            else
            {
                PIE1bits.TXIE = 0; //No data left to send: disable interrupt
            }
        }
    }

    //Check UART receive
    if (1==RCIF)
    {
        RCIF = 0;                          //Clear receive interrupt flag
        if (1==PIE1bits.RCIE)
        {
            //Received byte via UART
            //Save into buffer
            rcv.buf[rcv.head++] = RCREG;   //Save byte from UART into buffer
            if (rcv.head >= UART_BUF_RCV)
            {
                rcv.head = 0;
            }
            if (rcv.nB < UART_BUF_RCV)
            {
                rcv.nB++;                   //Update buffer count
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
static void StartXmit( void )
{
    //Check if there is no transmission in process
    if (0==PIE1bits.TXIE)
    {
        
        //Send next byte from buffer to UART peripheral
        TXREG = send.buf[send.tail++];
        if (send.tail >= UART_BUF_SEND)
        {
            send.tail = 0;
        }
        send.nB--;                  //Update buffer count
        PIE1bits.TXIE = 1;         //Enable transmission interrupt
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
UINT8 SendUART( char *str, UINT8 nBytes )
{
    //Save current buffer size so it doesn't change in an interrupt
    //Number of bytes available in buffer
    INT16 room = UART_BUF_SEND - (INT16)send.nB;
    UINT8 bytesLeft = 0;
    UINT8 i, gieh;
    
    if (room < 0)           //Not enough room in buffer?
    {
        nBytes += room;     //Don't add more than there is room for
        bytesLeft = 0-room;
    }

    for (i=0; i<nBytes; i++)
    {
        //PrintUs("Byte out: ", str[i]);
        //DebugPrint("\r\n");
        send.buf[send.head++] = str[i];
        if (send.head >= UART_BUF_SEND)
        {
            send.head = 0;
        }
    }

    INTCONbits.GIE = DISABLED;               //CRITICAL SECTION: DISABLE INTERRUPTS
    send.nB += nBytes;      //Update buffer count
    INTCONbits.GIE = ENABLED;               //CRITICAL SECTION: DISABLE INTERRUPTS

    StartXmit();            //Start transmission

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
UINT8 SendUARTcnst( const char *str, UINT8 nBytes )
{
    //Save current buffer size so it doesn't change in an interrupt
    //Number of bytes available in buffer
    INT16 room = UART_BUF_SEND - (INT16)send.nB;
    UINT8 bytesLeft = 0;
    UINT8 i, gieh;

    if (room < 0)           //Not enough room in buffer?
    {
        nBytes += room;     //Don't add more than there is room for
        bytesLeft = 0-room;
    }
    for (i=0; i<nBytes; i++)
    {
        send.buf[send.head++] = str[i];
        if (send.head >= UART_BUF_SEND)
        {
            send.head = 0;
        }
    }

    INTCONbits.GIE = DISABLED;               //CRITICAL SECTION: DISABLE INTERRUPTS
    send.nB += nBytes;                      //Update buffer count
    INTCONbits.GIE = ENABLED;               //CRITICAL SECTION: DISABLE INTERRUPTS

    StartXmit();            //Start transmission

    return bytesLeft;
}

//Function: GetNbytesReadyUART
//Inputs: none
//Outputs: unsigned char n - number of bytes in buffer
//
//Description:
//Returns the number of bytes currently available in the UART receive buffer.
UINT8 GetNbytesReadyUART( void )
{
    //PrintUs("N bytes ready = ", rcv.nB);
    //DebugPrint("\r\n");
    return rcv.nB;
}

//Function: ReadUART
//Inputs: char *str - pointer to data to send; UINT8 nBytes - N bytes to send
//Outputs:
//  BOOL results - TRUE means it read the requested number, FALSE means not
//  enough data was available to read
//
//Description:
//Reads the designated number of bytes out from the receive UART buffer.
BOOL ReadUART( char *str, UINT8 nBytes )
{
    BOOL retVal = FALSE;
    UINT8 gieh;

    if (rcv.nB>0)
    {
        if (rcv.nB < nBytes)
        {
            nBytes = rcv.nB;
        }
        else
        {
            retVal = TRUE;
        }

        UINT8 i;
        for (i=0; i<nBytes; i++)
        {
            str[i] = rcv.buf[rcv.tail++];
            if (rcv.tail >= UART_BUF_RCV)
            {
                rcv.tail = 0;
            }
        }
        
        INTCONbits.GIE = DISABLED;              //CRITICAL SECTION: DISABLE INTERRUPTS
        rcv.nB -= nBytes;       //Update buffer count
        INTCONbits.GIE = ENABLED;           //CRITICAL SECTION: DISABLE INTERRUPTS
    }

    return retVal;
}
