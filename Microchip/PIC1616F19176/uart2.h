//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: uart.h
//
//  Description:
//  This header file contains the UART driver function prototypes & definitions.
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#ifndef __UART2_H__
#define	__UART2_H__

#define UART2_BUF_RCV    40          //UART debug receive buffer size in bytes
#define UART2_BUF_SEND   250         //UART debug transmit buffer size in bytes

extern void InitializeUART2( void );
extern void HandleUART2int( void );
extern UINT8 SendUART2( char *str, UINT8 nBytes );
extern UINT8 SendUART2cnst( const char *str, UINT8 nBytes );
extern UINT8 GetNbytesReadyUART2( void );
extern BOOL ReadUART2( char *str, UINT8 nBytes );

#endif	//__UART_H__

