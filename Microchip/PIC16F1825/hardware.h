/**
	@Author: Joe Wierzbicki
	@Company: Westshore Design
	@File Name: hardware_PIC16F1825.h
	@Summary: This file contains hardware settings for PIC16F1825 micro.

*/

#ifndef HARDWARE_PIC16F1825_H
#define HARDWARE_PIC16F1825_H

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



#include <pic16f1825.h>

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


 /**** PORT A ******/
#define RA0DIR          OUTPUT
#define RA0TYPE         DIGITAL
#define TX_PGD          LATAbits.LATA0

#define RA1DIR          INPUT
#define RA1TYPE         DIGITAL
#define RX_PGC          PORTAbits.RA1

#define RA2DIR          OUTPUT
#define RA2TYPE         DIGITAL
#define SS_STR          LATAbits.LATA2    

#define RA4DIR          INPUT
#define RA4TYPE         DIGITAL             
#define PNP_IN          PORTAbits.RA4

#define RA5DIR          OUTPUT
#define RA5TYPE         DIGITAL
#define PWM_DIM         LATAbits.LATA5


 /**** PORT C ******/
#define RC0DIR          OUTPUT
#define RC0TYPE         DIGITAL
#define TX_EN           LATCbits.LATC0

#define RC1DIR          INPUT
#define RC1TYPE         DIGITAL
#define NPN_IN          PORTCbits.RC1

#define RC2DIR          INPUT
#define RC2TYPE         DIGITAL
#define COMP_IN         PORTCbits.RC2

#define RC3DIR          INPUT       
#define RC3TYPE         DIGITAL
#define AN_IN  	        PORTCbits.RC3

#define RC4DIR          OUTPUT
#define RC4TYPE         DIGITAL
#define OD_STR          LATCbits.LATC4

#define RC5DIR          OUTPUT
#define RC5TYPE         DIGITAL
#define GREEN_LED       LATCbits.LATC5

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
void HARDWARE_Initialize (INIT_STATE);
extern void EnableInterrupts();
extern void DisableInterrupts();

#endif // HARDWARE_PIC16F1825_H