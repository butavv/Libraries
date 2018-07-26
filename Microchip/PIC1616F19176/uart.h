
//**********************************************************************
//
//  SmartVision Lights Control Firmware
//
//  Compiler: XC8 v1.44
//
//  File: uart.c
//
//  Description:
//  This file contains header description for UART.c
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************
#ifndef __UART_H__
#define	__UART_H__

#define UART_BUF_RCV    40          //UART debug receive buffer size in bytes
#define UART_BUF_SEND   250         //UART debug transmit buffer size in bytes

extern void InitializeUART( void );
extern void HandleUARTint( void );
extern UINT8 SendUART( char *str, UINT8 nBytes );
extern UINT8 SendUARTcnst( const char *str, UINT8 nBytes );
extern UINT8 GetNbytesReadyUART( void );
extern BOOL ReadUART( char *str, UINT8 nBytes );

#endif	//__UART_H__

