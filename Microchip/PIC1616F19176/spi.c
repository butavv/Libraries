
/*
 *   SPI Driver for PIC16F19176
 *   6/20/2018
 *
 *   References: 
 *      Pages 517-562 of PIC16F19176 datasheet
 *      Page 4 MAX6682 datasheet
 */

#include "hardware.h"
#include "types.h"
#include "configuration.h"
#include "spi.h"
#include "timers.h"

static struct {
    spiState_t state;
    spiDevice_t device;
    UINT8 msb;
    UINT8 lsb;
    INT16 data;
}mSpiData;

//Function: InitializeMasterSPI
//Initializes SPI for PIC16F19176 in Master Mode
void InitializeMasterSpi( void )
{
    //Peripheral Pin Selection
    RB1PPS      = 0x13;         //Setup RB1 for SCK out
    SSP1DATPPS  = 0x0A;         //RB2 For Data In
    //SSP1SSPPS   = 0x20;         //RE0 NTC3_CS Slave Select out
    
    //Init Control Register
    SSP1CON1 = 0x0;             //Clear to start
    SSP1CON1bits.WCOL = 0;        //No Write Collision
    SSP1CON1bits.SSPOV = 0;       //Overlow not set
    SSP1CON1bits.SSPEN = 1;       //Enables serial port and configures SCK, SDO, SDI and SS as the source of the serial port pins
    SSP1CON1bits.CKP   = 0;       //Idle State for clock is a low level
    SSP1CON1bits.SSPM = 0b000;     //SPI Master mode, clock = Fosc/4
    
    //Init Status Register
    SSP1STATbits.SMP = 0;
    SSP1STATbits.CKE = 0;
    
    //Enable SSP Interrupts
    PIE3bits.SSP1IE = 1;
    
}

//Function: ReadSPI
//Reads data from the MAX6682MUA thermister chip
void StartSpi( spiDevice_t device )
{
    //Assign device
    mSpiData.device = device;
    
    NTC3_CS = LOW;   // Force CS Low
    SSP1BUF = 0x00; // Clear buffer to start transmission;
    mSpiData.state = SPI_READ1;
}

spiState_t GetSpiState()
{
    return mSpiData.state;
}


INT16 GetSpiData()
{
    mSpiData.state = SPI_IDLE;
    return mSpiData.data;
}

void HandleSpiInterrupt()
{
    if(SSP1IF == 1)
    {
        switch(mSpiData.state)
        {
            case SPI_IDLE:
                break;
            case SPI_READ1:
                if ( SSP1STATbits.BF == 1 )
                {
                    //NTC3_CS = HIGH; // Force CS High
                    HOT_SOL ^= ON;
                    if(SSP1BUF > 0)
                    {
                        CLD_SOL ^= ON;
                    }
                    mSpiData.msb = SSP1BUF; //reads buffer
                    SSP1BUF = 0x00; // Clear buffer to begin process;
                    //NTC3_CS = LOW; // Force CS Low
                    mSpiData.state = SPI_READ2;
                }
                break;
            case SPI_READ2:
                if ( SSP1STATbits.BF == 1 )
                {
                    if(SSP1BUF > 0)
                    {
                        DISP_SOL = ON;
                    }
                    mSpiData.lsb = SSP1BUF;
                    mSpiData.data = (mSpiData.msb<<8) | (mSpiData.lsb); //merge MSB and LSB 
                    mSpiData.data = mSpiData.data>>5; // Arithmetic shift 5 units to make signed 16 bit number
                    NTC3_CS = HIGH; // Force CS High
                    mSpiData.state = SPI_DONE;
                }
                break;
            case SPI_DONE:
                
                break;
        }
        
        //Clear interrupt flag
        SSP1IF = 0;
    }
}