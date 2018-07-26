
//**********************************************************************
//
//  Smart Vision Lights
//  
//  Compiler: XC8 v1.45
//
//  File: data.c
//
//  Description:
//  Implementation of LFSR to get random value 
//
//  2017 Phase 1 Engineering, LLC
//
//**********************************************************************

#include "hardware.h"
#include "types.h"
#include "configuration.h"
#include "uart.h"
#include "data.h"
#include "random.h"


UINT32 lfsr32, lfsr31;
BOOL seeded = FALSE; 

UINT32 shiftLFSR( UINT32 *lfsr, UINT32 mask)
{
    UINT32 feedback;
    
    feedback = *lfsr & 1;
    *lfsr >>= 1;
    
    if( feedback == 1 )
    {
        *lfsr ^= mask;
    }
    
    return *lfsr;
}

//Must be called before getRandom
void SeedLFSR( UINT32 seed_1, UINT32 seed_2)
{
    lfsr32 = seed_1;
    lfsr31 = seed_2;
    
    seeded = TRUE;
}

UINT32 GetRandom( void )
{
    if(seeded == FALSE)
    {
        return 0;
    }
    
    UINT32 retVal;
    UINT32 val32, val31;
    
    //Shift 32 bit LFSR once
    shiftLFSR( &lfsr32, MASK_32 );
    
    //Shift 32 bit LFSR again to get val32
    val32 = shiftLFSR( &lfsr32, MASK_32 );
    
    //Shift 31 bit LFSR
    val31 = shiftLFSR( &lfsr31, MASK_31 );
    
    UINT32 val = val32 ^ val31;
    
    //Only use bottom 16 bits
    UINT16 val16 = (val & 0xffff);

    //Scale 0 -500 ms
    UINT32 max = (UINT32)MAX_VALUE;
    UINT32 min = (UINT32)MIN_VALUE;

    return (val16 % (max - min +1) ) + min;
    
}