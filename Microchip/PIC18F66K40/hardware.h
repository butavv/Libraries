//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: hardware.h
//
//  Description:
//  This file contains hardware pin IO definitions and some defined constants
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#ifndef __HARDWARE_H__
#define	__HARDWARE_H__

#ifndef _18F66K40
#define _18F66K40
#endif

#include <pic18.h>

#define _XTAL_FREQ      64000000

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
#define ADCHD3			0b011011
#define ADCHG3          0b110011



////////////////////////////  Port A Pin Definitions  //////////////////////////

#define RA0DIR              INPUT               //1: Yaxis force in 1, analog
#define RA0TYPE             ANALOG
#define ADCH_YAFORCE1      	ADCHA0

#define RA1DIR              INPUT               //1: Yaxis force in 2, analog
#define RA1TYPE             ANALOG
#define ADCH_YAFORCE2      	ADCHA1

#define RA2DIR              INPUT               //1: Cushion force in, analog
#define RA2TYPE             ANALOG
#define ADCH_CUSHFORCE     	ADCHA2

#define RA3DIR              INPUT               //1: Spare load in 1, analog
#define RA3TYPE             ANALOG
#define ADCH_SPARELOAD1    	ADCHA3

#define RA4DIR              INPUT               //1: Spare load in 2, analog
#define RA4TYPE             ANALOG
#define ADCH_SPARELOAD2    	ADCHA4

#define RA5DIR              INPUT               //1: Y position in, analog
#define RA5TYPE             ANALOG
#define ADCH_YPOSITION     	ADCHA5

#define RA6DIR              INPUT               //1: Column position in, analog
#define RA6TYPE             ANALOG
#define ADCH_COLPOSITION   	ADCHA6

#define RA7DIR              INPUT               //1: Tension position in, analog
#define RA7TYPE             ANALOG
#define ADCH_TENSION       	ADCHA7


////////////////////////////  Port B Pin Definitions  //////////////////////////

#define RB0DIR              INPUT               //1: Lateral flex angle, analog
#define RB0TYPE             ANALOG
#define ADCH_LAT_FLEX      	ADCHB0         

#define RB1DIR              INPUT               //1: Flexion angle in, analog
#define RB1TYPE             ANALOG
#define ADCH_FLEXION       	ADCHB1

#define RB2DIR              INPUT               //1: Spare position in 1, analog
#define RB2TYPE             ANALOG
#define ADCH_SPAREPOS1     	ADCHB2

#define RB3DIR              INPUT               //1: Spare position in 2, analog
#define RB3TYPE             ANALOG
#define ADCH_SPAREPOS2     	ADCHB3

#define RB4DIR              INPUT               //1: Table height motor I sense, analog
#define RB4TYPE             ANALOG
#define ADCH_HEIGHT_ISEN   	ADCHB4

#define RB5DIR              INPUT               //1: Tension motor I sense, analog
#define RB5TYPE             ANALOG
#define ADCH_TENS_ISEN      ADCHB5

#define RB6DIR              OUTPUT               //0: Debug LED output / PGC Programming pin
#define RB6TYPE             DIGITAL
#define DEBUGLED	        LATBbits.LATB6

#define RB7DIR              OUTPUT               //0: LED driver chip select output / PGD programming pin
#define RB7TYPE             DIGITAL
#define LED_CS              LATBbits.LATB7



////////////////////////////  Port C Pin Definitions  //////////////////////////


#define RC0DIR          	OUTPUT               	//0: MOSI output
#define RC0TYPE				DIGITAL
#define SPI_MOSI			LATCbits.LATC0

#define RC1DIR          	INPUT               	//1: MISO input
#define RC1TYPE				DIGITAL
#define SPI_MISO			PORTCbits.RB1

#define RC2DIR          	OUTPUT               	//0: SPI CLK output
#define RC2TYPE				DIGITAL
#define SPI_CLK				LATCBits.LATC2

#define RC3DIR          	OUTPUT               	//0: Table height motor PWM output
#define RC3TYPE				DIGITAL
#define TAB_HEIGHT_PWM  	LATCbits.LATC3

#define RC4DIR          	OUTPUT               	//0: Tension motor PWM output
#define RC4TYPE				DIGITAL
#define TENSION_PWM     	LATCbits.LATC4

#define RC5DIR          	OUTPUT               	//0: Distraction motor PWM output
#define RC5TYPE				DIGITAL
#define DISTRACT_PWM    	LATCbits.LATC5

#define RC6DIR          	INPUT               	//1: Beaglebone UART input
#define RC6TYPE				DIGITAL
#define BB_UART_RX			PORTCbits.RC6

#define RC7DIR          	OUTPUT               		//0: Beaglebone UART output
#define RC7TYPE				DIGITAL
#define BB_UART_TX      	LATCbits.LATC7



////////////////////////////  Port D Pin Definitions  //////////////////////////

#define RD0DIR          	OUTPUT              //0: Debug UART out
#define RD0TYPE				DIGITAL
#define DBG_UART_TX     	LATDbits.LATD0

#define RD1DIR          	INPUT               //1: Debug UART in
#define RD1TYPE				DIGITAL
#define DBG_UART_RX			PORTDbits.RD1

#define RD2DIR          	OUTPUT              //0: Auto run done output
#define RD2TYPE             DIGITAL
#define AUTORUN         	LATDbits.LATD2


#define RD3DIR          	INPUT               //1: Spare motor current analog input
#define RD3TYPE           	ANALOG
#define ADCH_SPAREMOT_ISEN 	ADCHD3


#define RD4DIR          	OUTPUT              //0: Table height motor enable output
#define RD4TYPE				DIGITAL
#define TAB_HEIGHT_EN   	LATDbits.LATD4

#define RD5DIR          	OUTPUT              //0: Tension motor enable output
#define RD5TYPE				DIGITAL
#define TENSION_EN      	LATDbits.LATD5

#define RD6DIR          	OUTPUT               	//0: Distraction motor enable output
#define RD6TYPE				DIGITAL
#define DISTRACTION_EN  	LATDbits.LATD6

#define RD7DIR          	OUTPUT               	//0: LED Red Value Output
#define RD7TYPE				DIGITAL
#define LED_RVALUE	     	LATDbits.LATD7



////////////////////////////  Port E Pin Definitions  //////////////////////////

#define RE0DIR          	OUTPUT               //0: LED Green Value Output
#define RE0TYPE				DIGITAL
#define LED_GVALUE	    	LATEbits.LATE0

#define RE1DIR          	OUTPUT               //0: Table height motor SPI chip select
#define RE1TYPE				DIGITAL
#define TAB_HEIGHT_CS    	LATEbits.LATE1

#define RE2DIR          	OUTPUT               //0: Tension motor SPI chip select
#define RE2TYPE				DIGITAL
#define TENSION_CS      	LATEbits.LATE2

#define RE3DIR          	OUTPUT               //0: LED B Value Output
#define RE3TYPE				DIGITAL
#define LED_BVALUE 		   	LATEbits.LATE3

#define RE4DIR          	OUTPUT               //0: Table height motor direction
#define RE4TYPE				DIGITAL
#define TAB_HEIGHT_DIR   	LATEbits.LATE4

#define RE5DIR          	OUTPUT               //0: Tension motor direction
#define RE5TYPE				DIGITAL
#define TENSION_DIR     	LATEbits.LATE5

#define RE6DIR          	OUTPUT               //0: Distraction motor direction
#define RE6TYPE				DIGITAL
#define DISTRACT_DIR    	LATEbits.LATE6


#define RE7DIR          	OUTPUT               //0: LED Control Output
#define RE7TYPE				DIGITAL
#define BREAKOUT_IO	    	LATEbits.LATE7		



////////////////////////////  Port F Pin Definitions  //////////////////////////

#define RF0DIR          	INPUT               //1: Estop input (HIGH=Estop activated
#define RF0TYPE				DIGITAL
#define ESTOP           	PORTFbits.RF0

#define RF1DIR          	OUTPUT              //0: Motor power enable (HIGH=enabled)
#define RF1TYPE				DIGITAL
#define MOTORPOWER_EN   	LATFbits.LATF1

#define RF2DIR          	INPUT               //1: Manual Y axis from Tiller bar 
#define RF2TYPE				DIGITAL
#define MANUAL_Y       	    PORTFbits.RF2


#define RF3DIR          	INPUT               //1: Tension Down from Tiller bar
#define RF3TYPE				DIGITAL
#define TENS_DWN       	    PORTFbits.RF3

#define RF4DIR          	INPUT               //1: Column Up from Tiller bar
#define RF4TYPE 			DIGITAL
#define COL_UP       	    PORTFbits.RF4

#define RF5DIR          	OUTPUT               //1: Auto / Manual and Power off.  If HM contol holds high, Power on
#define RF5TYPE 			DIGITAL
#define AUTO_MAN       	    PORTFbits.RF5

#define RF6DIR          	INPUT               //1: Column Down from Tillr bar
#define RF6TYPE 			DIGITAL
#define COL_DWN       	    PORTFbits.RF6

#define RF7DIR          	INPUT               //1: Not used
#define RF7TYPE 			DIGITAL
#define TILLERIN6       	PORTFbits.RF7




////////////////////////////  Port G Pin Definitions  //////////////////////////

#define RG0DIR          	INPUT               //1: Tension Up from Tiller bar
#define RG0TYPE             DIGITAL
#define TENS_UP       	    PORTGbits.RG0


#define RG1DIR          	OUTPUT               //0: Aux out 2
#define RG1TYPE             DIGITAL
#define AUXOUT2         	LATGbits.LATG1


#define RG2DIR          	OUTPUT               //1: OUTPUT
#define RG2TYPE           	DIGITAL               //  Distraction Stepper Current
#define STEP_CURSET			LATGbits.LATG2

#define RG3DIR          	INPUT               //1: Stepper current read analog in
#define RG3TYPE           	ANALOG
#define ADCH_DISTRACT_ISEN  ADCHG3

#define RG4DIR          	OUTPUT              //0: Tiller output
#define RG4TYPE				DIGITAL
#define TILLER_OUT      	LATGbits.LATG4

#define RG6DIR          	INPUT              //1: Brake active input switch
#define RG6TYPE				DIGITAL
#define BRAKE_ACTIVE    	PORTGbits.RG6

#define RG7DIR          	INPUT               //1: Distraction RIGHT input switch
#define RG7TYPE				DIGITAL
#define DIST_RIGHT_IN   	PORTGbits.RG7



////////////////////////////  Port H Pin Definitions  //////////////////////////
#define RH0DIR        	INPUT              //1: Distraction LEFT input switch
#define RH0TYPE			DIGITAL
#define DIST_LEFT_IN    PORTHbits.RH0

#define RH1DIR          INPUT            //1: Column UP input switch
#define RH1TYPE			DIGITAL
#define COL_UP_IN       PORTHbits.RH1

#define RH2DIR        	INPUT             //1: Column DOWN input switch
#define RH2TYPE			DIGITAL
#define COL_DOWN_IN     PORTHbits.RH2

#define RH3DIR        	INPUT              //1: Pinch sensor input switch
#define RH3TYPE			DIGITAL
#define PINCH_IN        PORTHbits.RH3


#endif	//__HARDWARE_H__
