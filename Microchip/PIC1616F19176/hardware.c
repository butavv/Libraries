
/**
	@Author: Joe Wierzbicki
	@Company: Westshore Design
	@File Name: hardware_PIC16F1825.c
	@Summary: This file contains hardware settings for PIC16F1825 micro.

*/
#include <xc.h>
#include "hardware.h"
#include "timers.h"

/**

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
void InitializeHardware (INIT_STATE state)
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
			//PIE1bits.TMR1GIE = 0; //disable T1 gate interrupt	
			break;
		case POWER_UP:
		default:
            
            //Default to all digital
            ANSELA = 0;
            ANSELC = 0;
            
            //Pin Directions
            //Port A
            //Pin Directions
            //TRISAbits.TRISA0    = RA0DIR;
            TRISAbits.TRISA1    = RA1DIR;
            TRISAbits.TRISA2    = RA2DIR;
            TRISAbits.TRISA3    = RA3DIR;
            TRISAbits.TRISA4    = RA4DIR;
            //TRISAbits.TRISA5    = RA5DIR;
            //TRISAbits.TRISA6    = RA6DIR;
            //TRISAbits.TRISA7    = RA7DIR;
            
            //Analog/Digital Types
            //ANSELAbits.ANSA0  = RA0TYPE;
            ANSELAbits.ANSA1    = RA1TYPE;
            ANSELAbits.ANSA2    = RA2TYPE;
            ANSELAbits.ANSA3    = RA3TYPE;
            ANSELAbits.ANSA4    = RA4TYPE;
            
            
            //Port B
            //Pin Directions
            //TRISBbits.TRISB0    = RB0DIR;
            TRISBbits.TRISB1    = RB1DIR;
            TRISBbits.TRISB2    = RB2DIR;
            TRISBbits.TRISB3    = RB3DIR;
            TRISBbits.TRISB4    = RB4DIR;
            TRISBbits.TRISB5    = RB5DIR;
            TRISBbits.TRISB6    = RB6DIR;
            TRISBbits.TRISB7    = RB7DIR;
            
            //Analog/Digital Types
            //ANSELBbits.ANSB0  = RB0TYPE;
            ANSELBbits.ANSB1    = RB1TYPE;
            ANSELBbits.ANSB2    = RB2TYPE;
            ANSELBbits.ANSB3    = RB3TYPE;
            ANSELBbits.ANSB4    = RB4TYPE;
            ANSELBbits.ANSB5    = RB5TYPE;
            ANSELBbits.ANSB6    = RB6TYPE;
            ANSELBbits.ANSB7    = RB7TYPE;


            //Port C

            //Pin Directions
            //TRISCbits.TRISC0    = RC0DIR;
            //TRISCbits.TRISC1    = RC1DIR;
            TRISCbits.TRISC2    = RC2DIR;
            TRISCbits.TRISC3    = RC3DIR;
            TRISCbits.TRISC4    = RC4DIR;
            //TRISCbits.TRISC5    = RC5DIR;
            TRISCbits.TRISC6    = RC6DIR;
            TRISCbits.TRISC7    = RC7DIR;
            
            //Analog/Digital Types
            //ANSELCbits.ANSC0  = RC0TYPE;
            //ANSELCbits.ANSC1  = RC1TYPE;
            ANSELCbits.ANSC2    = RC2TYPE;
            ANSELCbits.ANSC3    = RC3TYPE;
            ANSELCbits.ANSC4    = RC4TYPE;
            //ANSELCbits.ANSC5  = RC5TYPE;
            ANSELCbits.ANSC6    = RC6TYPE;
            ANSELCbits.ANSC7    = RC7TYPE;
            
            
            
            //Analog/Digital Types
            ANSELDbits.ANSD0    = RD0TYPE;
            ANSELDbits.ANSD1    = RD1TYPE;
            ANSELDbits.ANSD2    = RD2TYPE;
            ANSELDbits.ANSD4    = RD4TYPE;
            ANSELDbits.ANSD5    = RD5TYPE;
            
            //Pin Directions
            TRISDbits.TRISD0    = RD0DIR;
            TRISDbits.TRISD1    = RD1DIR;
            TRISDbits.TRISD2    = RD2DIR;
            TRISDbits.TRISD3    = RD3DIR;
            TRISDbits.TRISD4    = RD4DIR;
            TRISDbits.TRISD5    = RD5DIR;
            
            ANSELEbits.ANSE0    = RE0TYPE;
            TRISEbits.TRISE0    = RE0DIR;
            
            //Output Initializations
            HOT_SOL = OFF;
            CLD_SOL = OFF;
            DISP_SOL = OFF;
            NTC3_CS = HIGH;
			
            //Timer Setup
            InitTimer2();
            InitTimer1();
            InitTimer4();
            ClearWdt();
            InitWdt();
            
			break;		
	}
}