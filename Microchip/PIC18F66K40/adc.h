//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: adcHandler.h
//
//  Description:
//  This header contains the ADC driver function declarations and definitions
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#ifndef __ADCHANDLER_H__
#define	__ADCHANDLER_H__

#define ADC_N_CHANNELS      16      //Total number of active channels
#define ADC_N_AVG           32      //Average this many samples
#define ADC_AVG_SHIFT       5       //Shift this many to divide by the average N
#define ADC_PRECHARGE       10      //Clock cycles of precharge time (to GND)
#define ADC_ACQTIME         10      //Clock cycles of acquisition (to signal in)
                                    //^^Units^^ = 2us per clock cycle
#define MAX_ADC_VAL             1024
#define MIN_ADC_VAL             0     
#define ADC_RANGE               (MAX_ADC_VAL - MIN_ADC_VAL)

typedef enum
{
    YAFORCE1 = 0,
    YAFORCE2,
    CUSHFORCE,
    SPARELOAD1,
    SPARELOAD2,
    Y_POSITION,
    COL_POSITION,
    TEN_POSITION,
    LAT_FLEX_ANG,
    FLEXION_ANG,
    SPAREPOS1,
    SPAREPOS2,
    TAB_HEIGHT_ISENSE,
    TENSION_ISENSE,
    DISTRACT_ISENSE            //(stepper motor current)
} adcChannel_t;

extern void InitializeADC( void );
extern void AdcInterrupt( void );
extern void adcHandler( void );
extern BOOL GetAdc( adcChannel_t adcChannel, UINT16 *latestVal, UINT16 *avgVal );
extern UINT16 getAdcAvg(adcChannel_t ch);
extern UINT16 getAdcLatest(adcChannel_t ch);
extern UINT16 scaleAdcVal(UINT16 adcVal, UINT16 min, UINT16 max);

#endif	//__ADCHANDLER_H__
