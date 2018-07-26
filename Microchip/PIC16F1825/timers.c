//**********************************************************************
//
//  SmartVision Lights Control Firmware
//
//  Compiler: XC8 v1.44
//
//  File: timers.c
//
//  Description:
//  This file contains all timer resources for PIC16F1825 micro
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************


#include "hardware.h"
#include "types.h"
#include "debug.h"
#include "string.h"
#include "configuration.h"
#include "timers.h"

#include <stdio.h>
#include <stdlib.h>

static struct
{
    UINT32 uptime_ms;           //Uptime counter in milliseconds
}sys;


void InitTimer1()
{
    // Setup Timer 1
    T1CON = 0x31;		// Timer1 8:1 (Instr clock), No prescale, ON
    T1GCON = 0x83;		// Gate En, Active low, Comparator 2 output.
    TMR1IE = 1;			// Enable TMR1 INT.
    C2IE = 1;				// Make sure Comp Int doesn't get disabled.
    INTCON = 0xE0;		// Global TMR0, and Peripheral.	
}

void InitTimer2()
{
    // Setup PWM & Timer2
    CCP1CON = 0x00;		// PWM off
    PR2 = 0xFE;			// Not FF, so there's no glitch at 100%
    T2CON = 0x04;		// Timer2 on, no Pre or Post scale
    CCP2CON = 0x0F;		// PWM Mode, p2a Active Low.
    CCP2AS = 0x00;		// No shutdown
    PWM2CON = 0x00;		// No Shutdown
    PSTR2CON = 0x01;	// P1A has output, 
    APFCON0	= 0x84;		// Steer RA1 is RX, RA0 is TX
    APFCON1	= 0x01;		// Steer RA5 is P2A
    CCPR2L = 0xFF;		// Set to no dim at startup   
}

//Setup Timer 4 as PWM resource as well as 1ms System Clock
//Period = 1ms 
void InitTimer4()
{
    T4CONbits.TMR4ON    = OFF;           //0:    Disable timer 4 to start
    T4CONbits.T4CKPS    = 0b01;          //0b01:  Prescaler = 1:4
    T4CONbits.T4OUTPS   = 0b1001;        //0b1001:   10x post-scaling
    PR4                 = 199;           //Count up this high before restart
    PIE3bits.TMR4IE     = ENABLED;       //1:    Enable timer 4 interrupt
}



void InitWdt()
{
    WDTCONbits.SWDTEN = ON;         //ON:   Enable watchdog timer
    WDTCONbits.WDTPS = 0b01111;
}

void ClearWdt()
{
    asm("CLRWDT");
}

void StartTimer1()
{
    T1CONbits.TMR1ON = ON;
}

void StopTimer1()
{
    T1CONbits.TMR1ON = OFF;
}

void StartTimer2()
{    
    T2CONbits.TMR2ON    = ON;            //1:    Enable timer 2
}

void StopTimer2()
{
    T2CONbits.TMR2ON    = OFF;            //1:    Enable timer 2
}

void StartTimer4()
{    
    T4CONbits.TMR4ON    = ON;            //1:    Enable timer 4
}

void StopTimer4()
{
    T4CONbits.TMR4ON    = OFF;            //1:    Enable timer 4
}

void HandleTimer4Interrupt()
{
    //Timer 4 interrupt
    //Occurs every 1ms
    if (TRUE == PIR3bits.TMR4IF)
    {
        sys.uptime_ms++;          //Tick system clock every 1ms
        PIR3bits.TMR4IF = 0;                 //Clear Timer 4 interrupt flag
    }
}

//Return system uptime in milliseconds
//ISR safe
UINT64 GetUptime( void )
{
    volatile UINT64 temp1, temp2;

    do  //Make sure the value doesn't change during access
    {
        temp1 = sys.uptime_ms;
        temp2 = sys.uptime_ms;
    } while (temp1 != temp2);

    return temp1;
}


void SysMsTick()
{
    sys.uptime_ms++;          //Tick system clock every 1ms
}
