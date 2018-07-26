//**********************************************************************
//
//
//  Smart Vision Lights: Lighting control firmware
//  
//  Compiler: XC8 v1.45
//
//  File: types.h
//
//  Description:
//  This is a header file containing variable type definitions.
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#ifndef __TYPES_H__
#define	__TYPES_H__

typedef unsigned char       UINT8;
typedef signed char         INT8;
typedef unsigned int        UINT16;
typedef signed int          INT16;
typedef unsigned long int   UINT32;
typedef signed long int     INT32;
typedef unsigned long long  UINT64;

typedef enum
{
    False = 0,
    True
} BOOL;

#define TRUE    1
#define FALSE   0


#endif	//__TYPES_H__