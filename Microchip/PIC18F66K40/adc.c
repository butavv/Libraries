//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: adcHandler.c
//
//  Description:
//  This file contains the ADC drivers.  Hardware resources used: ADC, timerX
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#include "hardware.h"
#include "types.h"
#include "configuration.h"
#include "uart.h"
#include "debug.h"
#include "timers.h"
#include "adcHandler.h"

//---------------------------- Local Definitions -------------------------------

//---------------------------- Static Variables --------------------------------
typedef struct
{   //Structure contains ADC control & data for a single input channel
    UINT16 latestVal;           //Most recent ADC reading
    UINT16 avgBuf[ADC_N_AVG];   //Buffer of most recent readings
    UINT16 runningAvg;          //Running sum of most recent readings
    UINT16 latestAvg;           //Latest calculated average reading
    UINT16 newReading;          //New reading taken from ADC
    UINT8 bufhead;              //Location to save next reading in avgBuf
    BOOL newValReady;           //Flag for accesors to know if data is updated
} adcChan_t;

static struct
{
    adcChan_t channel[ADC_N_CHANNELS];
    UINT8 currentChannel;
    BOOL newSample;
} adc;

//-------------------------- Function Definitions ------------------------------

static inline void selectAdcChan( void )
{   //Select input channel for the ADC
    
    switch( adc.currentChannel )
    {
        case 0:     //Fall through
        default:
            ADPCH = ADCH_YAFORCE1;
            break;
        case 1:
            ADPCH = ADCH_YAFORCE2;
            break;
        case 2:
            ADPCH = ADCH_CUSHFORCE;
            break;
        case 3:
            ADPCH = ADCH_SPARELOAD1;
            break;
        case 4:
            ADPCH = ADCH_SPARELOAD2;
            break;
        case 5:
            ADPCH = ADCH_YPOSITION;
            break;
        case 6:
            ADPCH = ADCH_COLPOSITION;
            break;
        case 7:
            ADPCH = ADCH_TENSION;
            break;
        case 8:
            ADPCH = ADCH_LAT_FLEX;
            break;
        case 9:
            ADPCH = ADCH_FLEXION;
            break;
        case 10:
            ADPCH = ADCH_SPAREPOS1;
            break;
        case 11:
            ADPCH = ADCH_SPAREPOS2;
            break;
        case 12:
            ADPCH = ADCH_HEIGHT_ISEN;
            break;
        case 13:
            ADPCH = ADCH_TENS_ISEN;
            break;
        case 14:
            ADPCH = ADCH_SPAREMOT_ISEN;
            break;
        case 15:
            ADPCH = ADCH_DISTRACT_ISEN;
            break;
    }
}

void InitializeADC( void )
{
    UINT8 i, j;
    
    DebugPrint("Initializing ADC\r\n");

    //Initialize/reset each channel
    for (i=0; i<ADC_N_CHANNELS; i++)
    {
        for (j=0; j<ADC_N_AVG; j++)
        {
            adc.channel[i].avgBuf[j] = 0;
        }
        adc.channel[i].bufhead = 0;
        adc.channel[i].latestAvg = 0;
        adc.channel[i].latestVal = 0;
        adc.channel[i].runningAvg = 0;
        adc.channel[i].newValReady = FALSE;
        
    }
    adc.currentChannel = 0;
    adc.newSample = FALSE;

    //Configure ADC
    ADCON0bits.ADON = 0;        //0: Turn off ADC while configuring
    PIE1bits.ADIE = 0;          //0: Interrupt disabled while configuring
    IPR1bits.ADIP = 1;          //High priority
    ADREFbits.ADNREF = 0;       //0: GND reference
    ADREFbits.ADPREF = 0;       //0: 3.3V reference
    ADCON0bits.ADCS = 0;        //0: FOSC clock source
    ADCLKbits.ADCS = 0b111111;  //111111: FOSC/128 clock rate
    ADCON0bits.ADFM = 1;        //1: right-justified results
    ADCON1bits.ADPPOL = 0;      //0: precharge to GND
    ADCON1bits.ADDSEN = 0;      //0: one conversion per trigger
    ADCON2bits.ADMD = 0;        //0: Basic sampling mode
    ADCON3bits.ADTMD = 0;       //0: No threshold interrupts
    
    DebugPrint("ADC Configured\r\n");
    PrintUs("Selecting Channel ", adc.currentChannel);
    selectAdcChan();            //First channel to read
    ADPRE = ADC_PRECHARGE;      //Precharge cycles

    ADACTbits.ADACT = 0b00010;  //0b00010 = Timer0 overflow triggers ADC convert
    
    DebugPrint("\r\nReady to start timer 0\r\n");
    initTimer0();
    DebugPrint("Timer 0 Initialized\r\n");
    startTimer0();
    DebugPrint("Timer 0 Started\r\n");
    
    PIR1bits.ADIF = 0;          //0: Clear interrupt flag
    PIE1bits.ADIE = 1;          //1: Enable interrupts
    DebugPrint("Interrupts Enabled\r\n");
    ADCON0bits.ADON = 1;        //1: Turn on ADC
    DebugPrint("ADC On\r\n");
    
}

void adcHandler( void )
{
    UINT8 i;

    if (TRUE == adc.newSample)
    {   //New samples read into the ADC buffers: save them & update averaging
        for (i=0; i<ADC_N_CHANNELS; i++)
        {
            //Get the raw value
            adc.channel[i].latestVal = adc.channel[i].newReading;  

            //Subtract the oldest value from the running sum
            adc.channel[i].runningAvg -=        
                    adc.channel[i].avgBuf[adc.channel[i].bufhead];
            
            //Add the raw value to the running sum
            adc.channel[i].runningAvg += adc.channel[i].newReading;
            
            //Divide the Running sum by 32 by right shifting 5 bits to get the average
            adc.channel[i].latestAvg =
                    adc.channel[i].runningAvg >> ADC_AVG_SHIFT;

            //Save raw value into buffer
            adc.channel[i].avgBuf[adc.channel[i].bufhead] = 
                    adc.channel[i].latestVal;
            
            //Increment the buffer head
            adc.channel[i].bufhead++;
            
            if (adc.channel[i].bufhead >= ADC_N_AVG)
            {
                adc.channel[i].bufhead = 0;
            }
            adc.channel[i].newValReady = TRUE;
        }

                
        ///BB Comment: ADC is never turned off.  It could be in the middle
        // of a sample right now and the interrupt that is generated might
        // not be for a reading of channel 0.
        
        //Ready for next set of samples
        adc.newSample = FALSE;
        adc.currentChannel = 0;     //Start back at first input channel
        selectAdcChan();
        PIE1bits.ADIE = 1;          //Enable interrupt; Timer0 will start ADC
    }
}

//High priority interrupt handler
void AdcInterrupt( void )
{
    if (1 == PIR1bits.ADIF)
    {   //ADC interrupt
        PIR1bits.ADIF = 0;      //Clear flag
        if (adc.currentChannel < ADC_N_CHANNELS)
        {   //Save new ADC reading
            adc.channel[adc.currentChannel].newReading = ADRESL;
            adc.channel[adc.currentChannel].newReading +=
                    ((UINT16)ADRESH)<<8;
            adc.currentChannel++;
            if (adc.currentChannel < ADC_N_CHANNELS)
            {
                selectAdcChan();            //Pick new input channel
                ADCON0bits.GO_nDONE = 1;    //Kick off new acquisition
            }
            else
            {
                adc.newSample = TRUE;       //Results ready for processing
                PIE1bits.ADIE = 0;          //Turn off ADC interrupt
            }
        }
        else
        {   //Invalid channel; start over
            adc.newSample = FALSE;
            adc.currentChannel = 0;
            selectAdcChan();
        }
    }
}

//Retrieve latest reading from the selected ADC channel
//Returns TRUE if it's a new reading or FALSE if hasn't been updated since the
//last time this function was called
BOOL GetAdc( adcChannel_t adcChannel, UINT16 *latestVal, UINT16 *avgVal )
{
    BOOL retVal = FALSE;

    if (adcChannel >= ADC_N_CHANNELS)
    {
        *latestVal = 0;
        *avgVal = 0;
    }
    else
    {
        *latestVal = adc.channel[adcChannel].latestVal;
        *avgVal = adc.channel[adcChannel].latestAvg;
        retVal = adc.channel[adcChannel].newValReady;
        adc.channel[adcChannel].newValReady = FALSE;
    }

    return retVal;
}


UINT16 getAdcAvg(adcChannel_t ch){
    UINT16 latest;
    UINT16 avg;
    GetAdc(ch, &latest, &avg);
    return avg;
}

UINT16 getAdcLatest(adcChannel_t ch){
    UINT16 latest;
    UINT16 avg;
    GetAdc(ch, &latest, &avg);
    return latest;
}

UINT16 scaleAdcVal(UINT16 adcVal, UINT16 min, UINT16 max)
{
    UINT32 range = max - min;
    UINT16 retVal = (UINT16) ( ((UINT32)(adcVal - (UINT16)(MIN_ADC_VAL)) * (range)) / (UINT32)(ADC_RANGE) );
    retVal += min;
    return retVal;
}