/**
	@Author: Joe Wierzbicki
	@Company: Westshore Design
	@File Name: hardware_PIC16F1825.c
	@Summary: This file contains hardware settings for PIC16F1825 micro.

*/
#include <xc.h>
#include "hardware.h"
#include "timers.h"

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
#pragma config PWRTE = OFF       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF       // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF          // Flash Program Memory Code Protection (Program memory code protection is enabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = OFF      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

/**
PINOUT based on PCB-00507-50

RA0 --> TX-PGD
RA1 --> RX-PGC
RA2 --> SS-STR
RA3 --> VPP
RA4 --> PNP-IN
RA5 --> PWM-DIM

RC0 --> TX-EN
RC1 --> NPN-IN
RC2 --> COMP-IN
RC3 --> 0-10V
RC4 --> OD-STR
RC5 --> RED-LED
*/

/**
 * @Param state
    The state of initialization determines how we want to reset the hardware.
 * @Returns
    none
 * @Description
    GPIO and peripheral I/O initialization
 * @Example
    HARDWARE_Initialize();
 */
void HARDWARE_Initialize (INIT_STATE state)
{
	switch(state)
	{
		//Feel free to change the peripheral settings here for boot load mode.
		case BOOTLOAD:
			PORTA = 0x20;   //0010 0000 : PWM-DIM pin high
			TRISA = 0x1A;   //0001 1010 :
			PORTC = 0x00;	//all low
			TRISC = 0x0E;	//0000 1110 : 0-10V, comp-in, and NPN-IN are inputs
			
			CM1CON0bits.C1ON = 0; //turn comparator 1 off
			CM2CON0bits.C2ON = 0; //turn comparator 2 off
			PIE2bits.C2IE = 0;  //disable comparator input for bootloader
			CCP1CON = 0x00;		//turn off CCP1
			CCP2CON = 0x00;		//turn off CCP2	
			
			//TODO: setup timers if need to be different.		
			T1GCON = 0x00; //turn off Timer1 gate	
			PIE1bits.TMR1GIE = 0; //disable T1 gate interrupt	
			break;
		case POWER_UP:
		default:
            
            //Setup Oscillator
            OSCCONbits.SCS = 00;
            OSCCONbits.IRCF = 0b1110;     // 8Mhz
            OSCCONbits.SPLLEN = 1;        // 4xPLL = 32MHz
            while(!OSCSTATbits.HFIOFR);
            while(!OSCSTATbits.HFIOFS);
            
            //Default to all digital
            ANSELA = 0;
            ANSELC = 0;
            
            //Pin Directions
            //Port A
            //Pin Directions
            TRISAbits.TRISA0    = RA0DIR;
            TRISAbits.TRISA1    = RA1DIR;
            TRISAbits.TRISA2    = RA2DIR;
            TRISAbits.TRISA4    = RA4DIR;
            TRISAbits.TRISA5    = RA5DIR;
            
            //Analog/Digital Types
            ANSELAbits.ANSA0  = RA0TYPE;
            ANSELAbits.ANSA1  = RA1TYPE;
            ANSELAbits.ANSA2  = RA2TYPE;
            ANSELAbits.ANSA4  = RA4TYPE;

            
            //Port C
            //Pin Directions
            TRISCbits.TRISC0    = RC0DIR;
            TRISCbits.TRISC1    = RC1DIR;
            TRISCbits.TRISC2    = RC2DIR;
            TRISCbits.TRISC3    = RC3DIR;
            TRISCbits.TRISC4    = RC4DIR;
            TRISCbits.TRISC5    = RC5DIR;
            
            //Analog/Digital Types
            ANSELCbits.ANSC0  = RC0TYPE;
            ANSELCbits.ANSC1  = RC1TYPE;
            ANSELCbits.ANSC2  = RC2TYPE;
            //ANSELCbits.  = RC4TYPE;
            
            TX_EN = ENABLED;
            
            //Global Interrupts Enable
            INTCONbits.GIE = ENABLED;
            INTCONbits.PEIE = ENABLED;
            
            
            //Initializations
            SS_STR = HIGH;
            OD_STR = HIGH;
            PWM_DIM = HIGH;
            GREEN_LED = OFF;
           
            
            /*
			OSCCON = 0x78;  //0111 1000 : 16Mhz INTOSC
			PORTA = 0x20;   //0010 0000 : PWM-DIM pin high
			TRISA = 0x1A;   //0001 1010 :
			PORTC = 0x00;	//all low
			TRISC = 0x0E;	//0000 1110 : 0-10V, comp-in, and NPN-IN are inputs
			OPTION_REG = 0x88; //weak pull ups disabled, no timer0 prescale
			
			//A2D setup
			ADCON0 = 0x05; //A2D on, channel 1
			ADCON1 = 0xD0; //Right justify, Fosc/16, AVdd for Ref
			ANSELA = 0x00; //no analog pins
			ANSELC = 0x08; //AN7 = 0-10V input
			
			//Comparator setup
			// BOTH comparators use the SAME input pin.  Each compatator need to be turned on and off depending on which mode.
			// C1OUT is used for Dimmed output
			// C2OUT is used for STROBED output
			CM1CON0 = 0xB6;		// ON, Pin enabled, Inverted output, High Speed, Hysteresis enabled, NOT Sync'd to TMR1
			CM1CON1 = 0xE2;		// BOTH Int, FVR for Ref, C1IN2- pin
			CM2CON0 = 0x96;		// ON, Pin DISabled, Inverted output, High Speed, Hysteresis enabled, NOT Sync'd to TMR1
			CM2CON1 = 0xE2;		// BOTH Int, FVR for Ref, C1IN2- pin
			FVRCON = 0x88;		// ON, 2.048V to Ref, OFF to ADC.
			*/
            //Timer Setup
			//InitTimer2();
            //InitTimer1();
            InitTimer4();
            ClearWdt();
            InitWdt();
            
             
			break;		
	}
    
   
}

 void EnableInterrupts()
    {
        //Global Interrupts Enable
        INTCONbits.GIE = ENABLED;
        //Peripheral Interrupts Enable
        INTCONbits.PEIE = ENABLED;
    }
    
    void DisableInterrupts()
    {
        //Global Interrupts Disable
        INTCONbits.GIE = ENABLED;
        //Peripheral Interrupts Disable
        INTCONbits.PEIE = ENABLED;
    }