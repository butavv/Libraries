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
#include "api.h"
#include "random.h"
#include <xc.h>


UINT16 ReadEEPROM(UINT16 address, BOOL cfg)
{
    
    
    if(cfg == TRUE){
        //Select Configuration Memory
        EECON1bits.CFGS = 1;
        EECON1bits.EEPGD = 1;
        EEADRL = (UINT8)address;
        EEADRH = 0;
    }
    else{
        //Select Program Memory
        EECON1bits.CFGS = 0;
        EECON1bits.EEPGD = 1;
        //Select Address
        EEADRL = (UINT8)address;
        EEADRH = (UINT8)((address >> 8) & 0xFF);
    }
    
    
    //Disable Interrupts
    DisableInterrupts();
    
    EECON1bits.RD = 1;  //Initiate Read
    asm("NOP");         //Execute Read
    asm("NOP");         //Complete Read
    
    //Enable Interrupts
    EnableInterrupts();
    
    UINT16 retVal;
    //Harvest EEPROM data
    retVal =  EEDATL;
    
    
    return retVal;
    
}


UINT8 ReadEEPROMHigh(UINT16 address)
{
    return (UINT8)((ReadEEPROM(address, FALSE) >> 8) & 0xFF);
}

UINT8 ReadEEPROMLow(UINT16 address)
{
    return (UINT8)(ReadEEPROM(address, FALSE) & 0xFF);
}

UINT8 ReadUserIdLsb_1()
{
    return (UINT8)(ReadEEPROM(0, TRUE) & 0xFF);
}

UINT8 ReadUserIdLsb_2()
{
    return (UINT8)(ReadEEPROM(1, TRUE) & 0xFF);
}

UINT8 ReadUserIdLsb_3()
{
    return (UINT8)(ReadEEPROM(2, TRUE) & 0xFF);
}

UINT8 ReadUserIdMsb()
{
    return (UINT8)(ReadEEPROM(3, TRUE) & 0xFF);
}

void WriteEEPROM(UINT8 address, UINT8 data)
{
    //Not Implemented
}