/**
	@Author: Joe Wierzbicki
	@Company: Westshore Design
	@File Name: hardware_PIC16F1825.h
	@Summary: This file contains hardware settings for PIC16F1825 micro.

*/

#ifndef HARDWARE_32MX664F064_H
#define HARDWARE_32MX664F064_H



// PIC16F1825 Configuration Bit Settings

// 'C' source line config statements
#define DEBUGON         0


#define _XTAL_FREQ      64000000

#define HIGH            1
#define LOW             0
#define ENABLED         1
#define DISABLED        0
#define ANALOG          0
#define DIGITAL         1
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


 /**** PORT B ******/
#define RB0DIR          OUTPUT
#define RB0TYPE         DIGITAL
#define PGD             LATBbits.LATB0

#define RB1DIR          OUTPUT
#define RB1TYPE         DIGITAL
#define PGC             LATBbits.RB1

#define RB2DIR          INPUT
#define RB2TYPE         DIGITAL
#define SW8             PORTBbits.RB2

#define RB3DIR          INPUT       
#define RB3TYPE         DIGITAL
#define SW7             PORTBbits.RB3

#define RB4DIR          OUTPUT
#define RB4TYPE         DIGITAL
#define SW6             LATCbits.LATC4

#define RB5DIR          INPUT
#define RB5TYPE         DIGITAL
#define SW5             PORTBbits.RB5

#define RB6DIR          OUTPUT
#define RB6TYPE         DIGITAL
#define LCD_DB0         LATBbits.LATB6

#define RB7DIR          INPUT
#define RB7TYPE         DIGITAL
#define LCD_DB1         PORTBbits.RB7

#define RB8DIR          OUTPUT
#define RB8TYPE         DIGITAL
#define LCD_DB2         LATBbits.LATB8

#define RB9DIR          OUTPUT
#define RB9TYPE         DIGITAL
#define LCD_DB3         LATBbits.LATB9

#define RB10DIR         OUTPUT
#define RB10TYPE        DIGITAL
#define LCD_DB4         LATBbits.LATB10

#define RB11DIR         OUTPUT
#define RB11TYPE        DIGITAL
#define LCD_DB5         LATBbits.LATB11

#define RB12DIR         OUTPUT
#define RB12TYPE        DIGITAL
#define LCD_DB6         LATBbits.LATB12

#define RB13DIR         OUTPUT
#define RB13TYPE        DIGITAL
#define LCD_DB7         LATBbits.LATB13

#define RB14DIR         OUTPUT
#define RB14TYPE        DIGITAL
#define LCD_FS          LATBbits.LATB14

#define RB15DIR         OUTPUT
#define RB15TYPE        DIGITAL
#define ETH_MDC         LATBbits.LATB15


 /**** PORT D ******/
#define RD0DIR          OUTPUT
#define RD0TYPE         DIGITAL
#define ETH_MINT        LATDbits.LATD0

#define RD1DIR          OUTPUT
#define RD1TYPE         DIGITAL
#define ETH_MDIO        LATDbits.LATD1

#define RD2DIR          OUTPUT
#define RD2TYPE         DIGITAL
#define RXD             LATDbits.LATD2

#define RD3DIR          OUTPUT       
#define RD3TYPE         DIGITAL
#define TXD             LATDbits.LATD3

#define RD4DIR          OUTPUT
#define RD4TYPE         DIGITAL
#define LCD_RD          LATDbits.LATD4

#define RD5DIR          OUTPUT
#define RD5TYPE         DIGITAL
#define LCD_CE          LATDbits.LATD5

#define RD6DIR          OUTPUT      
#define RD6TYPE         DIGITAL
#define LCD_CD          LATDbits.LATD6

#define RD7DIR          OUTPUT
#define RD7TYPE         DIGITAL
#define LCD_RES         LATDbits.LATD7

#define RD8DIR          OUTPUT
#define RD8TYPE         DIGITAL
#define LCD_WR          LATDbits.LATD8

#define RD9DIR          OUTPUT
#define RD9TYPE         DIGITAL
#define SDA             LATDbits.LATD9

#define RD10DIR         OUTPUT
#define RD10TYPE        DIGITAL
#define SCL             LATDbits.LATD10


 /**** PORT E ******/
#define RE0DIR          OUTPUT
#define RE0TYPE         DIGITAL
#define NTC3_CS         LATEbits.LATE0

#define RE1DIR          OUTPUT
#define RE1TYPE         DIGITAL
#define ETH_RXD0        LATEbits.LATE1

#define RE2DIR          OUTPUT
#define RE2TYPE         DIGITAL
#define ETH_CRSDV       LATEbits.LATE2

#define RE3DIR          OUTPUT
#define RE3TYPE         DIGITAL
#define RE3_NC          LATEbits.LATE3

#define RE4DIR          OUTPUT
#define RE4TYPE         DIGITAL
#define ETH_RXERR       LATEbits.LATE4

#define RE5DIR          OUTPUT
#define RE5TYPE         DIGITAL
#define ETH_TXEN        LATEbits.LATE5

#define RE6DIR          OUTPUT
#define RE6TYPE         DIGITAL
#define ETH_TXD0        LATEbits.LATE6
        
#define RE7DIR          OUTPUT
#define RE7TYPE         DIGITAL
#define ETH_TXD1        LATEbits.LATE7


/**** PORT F *****/
#define RF0DIR          OUTPUT
#define RF0TYPE         DIGITAL
#define HBT             LATFbits.LATF0

#define RF1DIR          OUTPUT
#define RF1TYPE         DIGITAL
#define BOOT            LATFbits.LATF1

#define RF3DIR          OUTPUT
#define RF3TYPE         DIGITAL
#define RF3_NC          LATFbits.LATF3

#define RF4DIR          OUTPUT
#define RF4TYPE         DIGITAL
#define RF4_NC          LATFbits.LATF4

#define RF5DIR          OUTPUT
#define RF5TYPE         DIGITAL
#define UART_DIR        LATFbits.LATF5


/**** PORT G ****/
#define RG2DIR          OUTPUT
#define RG2TYPE         DIGITAL
#define D_POS           LATGbits.LATG2

#define RG3DIR          OUTPUT
#define RG3TYPE         DIGITAL
#define D_NEG           LATGbits.LATG3
    
#define RG6DIR          OUTPUT
#define RG6TYPE         DIGITAL
#define SW1             LATGbits.LATG6

#define RG7DIR          OUTPUT
#define RG7TYPE         DIGITAL
#define SW2             LATGbits.LATG7

#define RG8DIR          OUTPUT
#define RG8TYPE         DIGITAL
#define SW3             LATGbits.LATG8

#define RG9DIR          OUTPUT
#define RG9TYPE         DIGITAL
#define SW4             LATGbits.LATG9



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