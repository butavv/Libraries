
/* 
 * File:   timers.c
 * Author: Nick
 *
 * Created on September 11, 2017, 3:12 PM
 */
#ifndef _18F66K40
#define _18F66K40
#endif

#include "configuration.h"
#include "hardware.h"
#include "types.h"
#include "debug.h"
#include "string.h"
#include "timers.h"

#include <stdio.h>
#include <stdlib.h>

static struct
{
    UINT32 uptime_ms;           //Uptime counter in milliseconds
}sys;

static struct
{
    UINT64 prevUptime;
    UINT64 secondsRemaining;
    UINT64 prevSecondsRemaining;
    countdownState_t state;
    BOOL tickReady;
}countdownTimer;

//Initilize Timer0 as the ADC driver timer
//Configure Timer0 with a 200Hz rollover rate to trigger the ADC
/*void setupTimer8()
{
TRISC5=1;
PWM7CON = 0;    
T8PR=250;
PWM7DCH =8;       
TMR8IF = 0;
T8CONbits.T8CKPS = 4; // 1:1 PRESCALER
T8CONbits.T8OUTPS = 0; // 1:1 Output postscaler
T8CONbits.T8ON = 1;
CCPTMRS1bits.P7TSEL = 3;   // PWM 7 is based of Timer 8
RC5PPS = 0xB;              // Setup RC5 as PWM7 output
TRISC5 = 0;
T8CLKCON = 1;  // FOSC/4
PWM7CONbits.EN = 1;
}*/
void initTimer0()
{
    T0CON0bits.T0EN     = DISABLED;        //0: Disable to start
    T0CON0bits.T016BIT  = 0;               //0: 8-bit timer/compare
    T0CON0bits.T0OUTPS  = 0;               //0: No postscaler
    T0CON1bits.T0CS     = 0b010;           //0b010: Fosc/4 clock source (16MHz)
    T0CON1bits.T0CKPS   = 11;              //11: 1:2048 prescale (7812.5Hz)
    TMR0H = 38;                            //38: divide 7812.5 by (38+1) = 200.3Hz rollover
}

void initTimer1()
{
    
}

//Setup Timer 2 as PWM resource as well as 1ms System Clock
//Period = 1ms 
void initTimer2()
{
    sys.uptime_ms = 0;
    
    T2CONbits.TMR2ON    = OFF;           //0:    Disable timer 2 to start
    T2CONbits.T2CKPS    = 0b011;         //0b010:  Prescaler = 1:8
    T2CONbits.T2OUTPS   = 0b1001;        //0b0011:   10x post-scaling
    T2HLTbits.MODE      = 0;             //0: Period running, SW only
    T2CLKCONbits.CS     = 0b0001;        //0b001: FOSC/4 (16MHz)
    T2HLTbits.PSYNC     = 1;
            
    T2PR                = 199;           //Count up this high before restart
    IPR5bits.TMR2IP     = 1;             //1:    High interrupt priority
    TMR2IE              = ENABLED;            //1:    Enable timer 2 interrupt
}

void initTimer3()
{
    
}

void initWdt(){
    WDTCON0bits.SWDTEN = ON;         //ON:   Enable watchdog timer
}

void clearWatchdogTimer(){
    ClrWdt();
}

void InitCountdownTimer(UINT64 seconds)
{
    countdownTimer.secondsRemaining = seconds;
    countdownTimer.state = COUNTDOWN_IDLE;
}

void StartCountdownTimer()
{
    countdownTimer.state = COUNTDOWN_RUNNING;
}

void PauseCountdownTimer()
{
    countdownTimer.state = COUNTDOWN_PAUSED;
}

void StopCountdownTimer()
{
    countdownTimer.state = COUNTDOWN_IDLE;
}

void HandleCountdownTimer()
{
    UINT64 elapsedMillis;
    switch(countdownTimer.state)
    {
        case COUNTDOWN_RUNNING:
            elapsedMillis = GetUptime() - countdownTimer.prevUptime;
            countdownTimer.secondsRemaining = countdownTimer.secondsRemaining - (elapsedMillis/1000);
            if( countdownTimer.secondsRemaining != countdownTimer.prevSecondsRemaining){
                countdownTimer.prevSecondsRemaining = countdownTimer.secondsRemaining;
                countdownTimer.prevUptime = GetUptime();
                countdownTimer.tickReady = TRUE;
            }
            break;
        case COUNTDOWN_PAUSED:
            countdownTimer.prevUptime = GetUptime();
            break;
        case COUNTDOWN_IDLE:
            countdownTimer.prevUptime = GetUptime();
            break;
    }
}

countdownState_t GetCountdownTimerState()
{
    return countdownTimer.state;
}

UINT64 GetCountdownTimerSecondsRemaining()
{
    return countdownTimer.secondsRemaining;
}

void SetCountdownTimerSecondsRemaining(UINT64 seconds)
{
    countdownTimer.secondsRemaining = seconds;
}

BOOL GetCountdownTimerTickReady()
{
    return countdownTimer.tickReady;
}

void ClearCountdownTimerTickReady()
{
    countdownTimer.tickReady = FALSE;
}

void initAllTimers()
{
    initTimer1();
    initTimer2();
    initTimer3();
}

void startTimer0()
{
    T0CON0bits.T0EN = ENABLED;        //1: Enable Timer0 to kick off ADC sampling
}

void stopTimer0(){
    T0CON0bits.T0EN = DISABLED;        //1: Disable Timer0 
}

void startTimer1(){

}

void stopTimer1(){

}

void startTimer2(){
    
    T2CONbits.TMR2ON    = ENABLED;            //1:    Enable timer 2
}

void stopTimer2(){
    T2CONbits.TMR2ON    = DISABLED;            //1:    Enable timer 2
}

void UptimeTick()
{
    sys.uptime_ms++;          //Tick system clock every 1ms
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