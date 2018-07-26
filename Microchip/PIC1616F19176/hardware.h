/**
	@Author: Joe Wierzbicki
	@Company: Westshore Design
	@File Name: hardware_PIC16F1825.h
	@Summary: This file contains hardware settings for PIC16F1825 micro.

*/

#ifndef HARDWARE_16F19176_H
#define HARDWARE_16F19176_H

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

// PIC16F1825 Configuration Bit Settings

// 'C' source line config statements
#define DEBUGON         0

// CONFIG1
#pragma config RSTOSC = HFINTPLL    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
//#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = ON          // Flash Program Memory Code Protection (Program memory code protection is enabled)
//#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
//#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
//#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
//#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
#pragma config CP = ON           //OFF: User NVM not protected


#include <pic16f19176.h>

#define _XTAL_FREQ      32000000

#define HIGH            1
#define LOW             0
#define ON              1
#define OFF             0
#define ENABLED         1
#define DISABLED        0
#define ANALOG          1
#define DIGITAL         0
#define INPUT           1
#define OUTPUT          0
#define TX              1
#define RX              0


#define ADCHA0 			0b000000
#define ADCHA1 			0b000001
#define ADCHA2 			0b000010
#define ADCHA3 			0b000011
#define ADCHA4 			0b000100
#define ADCHA5 			0b000101	
#define ADCHA6 			0b000110
#define ADCHA7			0b000111	
#define ADCHB0			0b001000
#define ADCHB1			0b001001
#define ADCHB2 			0b001010
#define ADCHB3 			0b001011
#define ADCHB4 			0b001100
#define ADCHB5          0b001101
#define ADCHB6			0b011011
#define ADCHB7          0b110011
#define ADCHC0			0b001000
#define ADCHC1			0b001001
#define ADCHC2 			0b001010
#define ADCHC3 			0b001011
#define ADCHC4 			0b001100
#define ADCHC5          0b001101
#define ADCHC6			0b011011
#define ADCHC7          0b110011
#define ADCHD0			0b001000
#define ADCHD1			0b001001
#define ADCHD2 			0b001010
#define ADCHD3 			0b001011
#define ADCHD4 			0b001100
#define ADCHD5          0b001101
#define ADCHD6			0b011011
#define ADCHD7          0b110011

 /**** PORT A ******/
//#define RA0DIR          OUTPUT                  //Not Connected
//#define RA0TYPE         DIGITAL
//#define RA0_NC          LATAbits.LATA0

#define RA1DIR          OUTPUT                    //NTC2_CS
#define RA1TYPE         DIGITAL
#define NTC2_CS         LATAbits.LATA1

#define RA2DIR          OUTPUT                  //Pump On/Off
#define RA2TYPE         DIGITAL
#define PUMP            LATAbits.LATA2   

#define RA3DIR          OUTPUT                   //Cold Water Solenoid
#define RA3TYPE         DIGITAL             
#define CLD_SOL         LATAbits.LATA3

#define RA4DIR          OUTPUT
#define RA4TYPE         DIGITAL             
#define NTC1_CS         LATAbits.LATA4

/*
#define RA5DIR          OUTPUT
#define RA5TYPE         DIGITAL
#define RA1_NC          LATAbits.LATA5
 * 
#define RA6DIR          OUTPUT
#define RA6TYPE         DIGITAL
#define RA6             LATAbits.LATA5
  
#define RA7DIR          OUTPUT
#define RA7TYPE         DIGITAL
#define RA7             LATAbits.LATA5
*/

 /**** PORT B ******/
/*
#define RB0DIR          OUTPUT
#define RB0TYPE         DIGITAL
#define RB0_NC          LATBbits.LATB0
*/

#define RB1DIR          OUTPUT
#define RB1TYPE         DIGITAL
#define NTC3_CLK        LATBbits.RB1

#define RB2DIR          INPUT
#define RB2TYPE         DIGITAL
#define NTC3_DATA       PORTBbits.RB2

#define RB3DIR          INPUT       
#define RB3TYPE         DIGITAL
#define R1              PORTBbits.RB3

#define RB4DIR          OUTPUT
#define RB4TYPE         DIGITAL
#define RB4_NC          LATCbits.LATC4

#define RB5DIR          INPUT
#define RB5TYPE         DIGITAL
#define BOOT            PORTBbits.RB5

#define RB6DIR          OUTPUT
#define RB6TYPE         DIGITAL
#define ICSP_CLK        LATBbits.LATB6

#define RB7DIR          INPUT
#define RB7TYPE         DIGITAL
#define ICSP_DATA       PORTBbits.RB7




 /**** PORT C ******/
/*#define RC0DIR          OUTPUT
#define RC0TYPE         DIGITAL
#define TX_EN           LATCbits.LATC0

#define RC1DIR          INPUT
#define RC1TYPE         DIGITAL
#define NPN_IN          PORTCbits.RC1
*/
 
#define RC2DIR          OUTPUT
#define RC2TYPE         DIGITAL
#define DISP_SOL        LATCbits.LATC2

#define RC3DIR          OUTPUT       
#define RC3TYPE         DIGITAL
#define HOT_SOL         LATCbits.LATC3

#define RC4DIR          INPUT
#define RC4TYPE         ANALOG
#define ADCH_H_REF_PSR  ADCHC4

/*
#define RC5DIR          OUTPUT
#define RC5TYPE         DIGITAL
#define RED_LED         LATCbits.LATC5
*/

#define RC6DIR          OUTPUT
#define RC6TYPE         DIGITAL
#define UART_TX         LATCbits.LATC6 

#define RC7DIR          INPUT
#define RC7TYPE         DIGITAL
#define UART_RX         PORTCbits.RC7


 /**** PORT D ******/
#define RD0DIR          INPUT
#define RD0TYPE         ANALOG
#define ADCH_TNK_LVL    ADCHD0

#define RD1DIR          INPUT
#define RD1TYPE         ANALOG
#define ADCH_PRESSURE_V ADCHD1

#define RD2DIR          INPUT
#define RD2TYPE         ANALOG
#define ADCH_SPARE      ADCHD2

#define RD3DIR          OUTPUT       
#define RD3TYPE         DIGITAL
#define FAN		        LATDbits.LATD3

#define RD4DIR          OUTPUT
#define RD4TYPE         DIGITAL
#define UART_DIR        LATDbits.LATD4

#define RD5DIR          INPUT
#define RD5TYPE         ANALOG
#define ADCH_BCKT_SNR   ADCHD5


 /**** PORT E ******/
#define RE0DIR          OUTPUT
#define RE0TYPE         DIGITAL
#define NTC3_CS         LATEbits.LATE0


typedef enum 
{
	POWER_UP,
	BOOTLOAD
}INIT_STATE;


/**
 * @Param INIT_STATE
    The state of initialization determines how we want to reset the hardware.
 * @Returns
    none
 * @Description
    GPIO and peripheral I/O initialization
 * @Example
    HARDWARE_Initialize();
 */
void InitializeHardware (INIT_STATE);

#endif // HARDWARE_PIC16F1825_H