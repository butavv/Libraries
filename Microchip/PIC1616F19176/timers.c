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
}

//Setup Timer 4 as PWM resource as well as 1ms System Clock
//HFINTFOSC = 32MHz
//HFINTFOSC/4 = 8MHz
//Period = 1ms 
void InitTimer4()
{
    T4CONbits.TMR4ON    = OFF;           //0:    Disable timer 4 to start
    T4CLKCONbits.CS     = 0b0011;         //HFINTOSC = 32MHz
    //T4HLTbits.PSYNC         = 0b1;              //Prescalar is synchronized to Fosc/4
    //T4HLTbits.MODE          = 0b00000;
    T4CONbits.T4CKPS    = 0b100;         //0b010:  Prescaler = 1:16
    T4CONbits.T4OUTPS   = 0b1001;        //0b0011:   10x post-scaling
    PR4                 = 199;           //Count up this high before restart
    //IPR5bits.TMR2IP     = 1;             //1:    High interrupt priority
    TMR4IE              = ENABLED;            //1:    Enable timer 4 interrupt
}



void InitWdt()
{
    WDTCON0bits.SWDTEN = ON;         //ON:   Enable watchdog timer
}

void ClearWdt()
{
    asm("clrwdt"); 
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
    T4CONbits.TMR4ON    = OFF;            //1:    Enable timer 2
}

void HandleTimer4Interrupt(){
    //Timer 4 interrupt
    //Occurs every 1ms
    if (TRUE == TMR4IF)
    {
        sys.uptime_ms++;          //Tick system clock every 1ms
        TMR4IF = 0;                 //Clear Timer 2 interrupt flag
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
    sys.uptime_ms++;
}
