
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
PINOUT based on PCB-00507-50

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
// CONFIG1
#pragma config USERID       = 00000000      //User ID = 0
#pragma config FSRSSEL      = PRIORITY_7    //Interrupt Priority 7
#pragma config FMIIEN       = ON            //Ethernet Enabled
#pragma config FETHIO       = ON            //Default Ethernet I/O
#pragma config FUSBIDIO     = ON            //USB VBUS Controlled by USB module
#pragma config FPLLIDIV     = DIV_2         //2x Input Divider
#pragma config UPLLIDIV     = DIV_2         //2x USB PLL Divider
#pragma config UPLLEN       = OFF           //USB PLL OFF
#pragma config FPLLODIV     = DIV_2         //2x Output Divider
#pragma config FNOSC        = PRIPLL        //Primary Osc PLL
#pragma config FSOSCEN      = OFF           //Secondary Oscillator off
#pragma config IESO         = OFF           //Internal Switch Over Disabled
#pragma config POSCMOD      = HS            //Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC     = OFF           //CLKO OUtput Signal not active on OSCO pin
#pragma config FPBDIV       = DIV_1         //Peripheral clock divisor 1x
#pragma config FCKSM        = CSECME        //Clock Switch Disable, FSCM Disabled
#pragma config WDTPS        = PS1048576     //1:1048576 Watchdog postscalar
#pragma config FWDTEN       = OFF           //Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config DEBUG        = ON            //Debugger Enabled
#pragma config ICESEL       = ICS_PGx1      //ICE EMUC1/EMD1 pins shared with PGC1/PGD1
#pragma config PWP          = OFF           //Disable write protect
#pragma config BWP          = OFF           //Boot Flash Write protection disabled
#pragma config CP           = OFF           //Code Protection disabled
 
void InitializeHardware (INIT_STATE state)
{
	switch(state)
	{
		//Feel free to change the peripheral settings here for boot load mode.
		case BOOTLOAD:
			/*PORTA = 0x20;   //0010 0000 : PWM-DIM pin high
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
			//PIE1bits.TMR1GIE = 0; //disable T1 gate interrupt	*/
			break;
		case POWER_UP:
		default:
            
            //Default to digital
            AD1PCFG = 0x00;
   
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
            TRISBbits.TRISB8    = RB8DIR;
            TRISBbits.TRISB9    = RB9DIR;
            TRISBbits.TRISB10   = RB10DIR;
            TRISBbits.TRISB11   = RB11DIR;
            TRISBbits.TRISB12   = RB12DIR;
            TRISBbits.TRISB13   = RB13DIR;
            TRISBbits.TRISB14   = RB14DIR;
            TRISBbits.TRISB15   = RB15DIR;
            
            //Pin Directions
            TRISDbits.TRISD0    = RD0DIR;
            TRISDbits.TRISD1    = RD1DIR;
            TRISDbits.TRISD2    = RD2DIR;
            TRISDbits.TRISD3    = RD3DIR;
            TRISDbits.TRISD4    = RD4DIR;
            TRISDbits.TRISD5    = RD5DIR;
            TRISDbits.TRISD6    = RD6DIR;
            TRISDbits.TRISD7    = RD7DIR;
            TRISDbits.TRISD8    = RD8DIR;
            TRISDbits.TRISD9    = RD9DIR;
            TRISDbits.TRISD10   = RD10DIR;

            //Pin Directions
            TRISEbits.TRISE0    = RE0DIR;
            TRISEbits.TRISE1    = RE1DIR;
            TRISEbits.TRISE2    = RE2DIR;
            TRISEbits.TRISE3    = RE3DIR;
            TRISEbits.TRISE4    = RE4DIR;
            TRISEbits.TRISE5    = RE5DIR;
            TRISEbits.TRISE6    = RE6DIR;
            TRISEbits.TRISE7    = RE7DIR;


            //Pin Directions
            TRISFbits.TRISF0    = RF0DIR;
            TRISFbits.TRISF1    = RF1DIR;

            TRISFbits.TRISF3    = RF3DIR;
            TRISFbits.TRISF4    = RF4DIR;
            TRISFbits.TRISF5    = RF5DIR;


            
			
            //Timer Setup
			InitTimer2();
            InitTimer1();
            InitTimer4();
            ClearWdt();
            InitWdt();
            
			break;		
	}
}
