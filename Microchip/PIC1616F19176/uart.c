

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
{   //Set baud rate 115200: 32MHz/(4 * (68+1) = 115,107 (-0.08%)
    TX1STAbits.BRGH = 1;                //1: High-speed mode (divide by 4)
    BAUD1CONbits.BRG16 = 1;             //1: 16-bit baud rate generator
    BAUD1CONbits.ABDEN = 0;             //0: No auto-baud
    BAUD1CONbits.SCKP = 0;              //0: Idle high
    SP1BRGH = 0;                        // high byte = 1
    SP1BRGL = 68;                       //68: divide by 68+1
    //BAUDCONbits.CKTXP = 0;             //0: Idle TX state is high
    
    //Initialize receive buffer
    rcv.head = 0;
    rcv.tail = 0;
    rcv.nB = 0;

    //Initialize UART transmit buffer
    send.head = 0;
    send.tail = 0;
    send.nB = 0;

    //Configure UART
    RC1STAbits.RX9 = 0;                 //0: 8-bit reception
    RC1STAbits.ADDEN = 0;               //0: Disable address detection
    TX1STAbits.SYNC = 0;                //0: Asynchronous
    TX1STAbits.TX9 = 0;                 //0: 8-bit data mode
    RC1STAbits.FERR = 0;                //0: Clear framing error
    RC1STAbits.OERR = 0;                //0: Clear overrun error
    //IPRbits.RC1IP = 1;                 //1: High interrupt priority (RX)
    //IPRbits.TX1IP = 1;                 //1: High interrupt priority (TX)
    PIE3bits.RC1IE = 1;                 //1: Enable RX interrupts
    PIE3bits.TX1IE = 1;                  //1: Enable TX interrupts
    
    //Pin Peripheral selection
    RX1PPS = 0x17;    //C7
    RC6PPS = 0x0D;    //TX1


    //Turn on the UART
    RC1STAbits.CREN = 1;                //1: Enable receiver
    TX1STAbits.TXEN = 1;                //1: Enable transmitter
    RC1STAbits.SPEN = 1;
    //Enable transmit complete interrupt after a byte was sent, not here
    //PIE1bits.TX1IE = 1;
    
    UART_DIR = RX;
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
    if (1==TX1IF)
    {
        TX1IF = 0;                   //Clear transmit interrupt flag
        if (1==PIE3bits.TX1IE)
        {
            //Finished transmitting. Is there another byte to send?
            if (send.nB > 0)
            {
                //Send next byte to buffer
                TX1REG = send.buf[send.tail++];
                if (send.tail >= UART_BUF_SEND)
                {
                    send.tail = 0;
                }
                //Update buffer count
                send.nB--;
            }
            else
            {
                PIE3bits.TX1IE = 0; //No data left to send: disable interrupt
                UART_DIR = RX;       //Start Listening
                CLD_SOL = OFF;
            }
        }
    }

    //Check UART receive
    if (1==RC1IF)
    {
        RC1IF = 0;                          //Clear receive interrupt flag
        if (1==PIE3bits.RC1IE)
        {
            //Received byte via UART
            //Save into buffer
            rcv.buf[rcv.head++] = RC1REG;   //Save byte from UART into buffer
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
    if (0==PIE3bits.TX1IE)
    {
        UART_DIR = TX;          //Enable transmission
        //CLD_SOL = ON;
    
        //Send next byte from buffer to UART peripheral
        TX1REG = send.buf[send.tail++];
        if (send.tail >= UART_BUF_SEND)
        {
            send.tail = 0;
        }
        send.nB--;                  //Update buffer count
        PIE3bits.TX1IE = 1;         //Enable transmission interrupt
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
