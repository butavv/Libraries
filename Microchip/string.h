//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: string.h
//
//  Description:
//  This file is a header for functions to read and write numeric ASCII strings.
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#ifndef __STRING_H__
#define	__STRING_H__

#include "types.h"

extern UINT8 usitoa( UINT16 val, char *str, UINT8 n, BOOL pad, UINT8 padChar );
extern void usitoa4( UINT16 val, char *str );
extern void usitoa5( UINT16 val, char *str );   
extern void usitoa8( UINT32 val, char *str );
extern UINT16 atousi4( char *str );
extern UINT16 atousi5( char *str );
extern UINT32 atousi8( char *str );
extern UINT8 htousi2( char *str );
extern BOOL IsHexChar( char myChar );
extern BOOL cmp( char *str1, char *str2, UINT8 len );
extern BOOL isAlphaNumeric( char byte );

#endif	//__STRING_H__

