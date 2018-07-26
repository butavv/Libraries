//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: debug.c
//
//  Description:
//  This file contains debugging functions.
//  Hardware resources used: none
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************
#include "configuration.h"
#include "hardware.h"
#include "types.h"
#include "debug.h"
#include "string.h"
#include "uart2.h"
#include "api.h"
#include "adcHandler.h"
#include "timers.h"

//---------------------------- Local Definitions -------------------------------

typedef enum                //Receive UART debug command state
{
    DBG_RDY,                //No characters received
    DBG_RCV,                //Receiving data
}dbgMsgState_t;

//---------------------------- Static Variables --------------------------------
static struct               //Debug control and status
{
    BOOL blink;             //Debug blink status
    BOOL enabled;           //Debugging function status
    BOOL blinkMl;           //If TRUE, blink during main loop handling
    char rcv[DBG_NCHAR];    //Command receive buffer
    UINT8 nRcv;             //Number of received command bytes
    UINT8 nExp;             //Number of expected command bytes
    dbgMsgState_t state;    //State of received characters from UART

    UINT64 blinkMs;         //Blink timer, milliseconds
} debug;

//-------------------------- Function Definitions ------------------------------

//Function: InitializeDebugging
//Inputs: none
//Outputs: none
//
//Description:
//Set up debug UART handler
void InitializeDebugging( void )
{
    debug.blink = ON;                   //Turn on debug blinker
    debug.blinkMs = BLINK_RATE_MS;
    debug.enabled = ON;                 //Debugging defaults to ON
    debug.nRcv = 0;                     //0 bytes in receive buffer
    debug.blinkMl = FALSE;
}

//-------------------------------  Message handling ----------------------------

//Function: DebugPrint
//Inputs: char *str - pointer to data to send; must be null-terminated
//Outputs: none
//
//Description:
//Prints a string out the debug UART.  Must be null-terminated.
//Example: DebugPrint( "Hello world!\r\n" );
void DebugPrint( const char* dbgStr )
{
    UINT16 i;
    if (TRUE == debug.enabled)
    {
        for (i=0; i<MAX_STRING; i++)
        {
            if (0 == dbgStr[i])
            {
                break;
            }
        }
        if (i>0)
        {
            (void)SendUART2cnst( dbgStr, i );
        }
    }
}

//Function: PrintU
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          UINT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and number out the debug UART. String must be null-terminated.
//Example: PrintU( "Value=", 349 ); results in "Value=00349".
void PrintU( const char *str, UINT16 val )
{
    char valStr[] = "00000";

    if (TRUE == debug.enabled)
    {
        DebugPrint( str );
        usitoa5( val, valStr );
        SendUART2( valStr, sizeof(valStr)-1 );   //Don't send null termination
    }
}

void PrintTableState(tableState_t tState)
{
    switch(tState)
    {
        case IDLE:
            DebugPrint("IDLE");
            break;
        case RUN_MANUAL:
            DebugPrint("RUN_MANUAL");
            break;
        case PAUSE_MANUAL:
            DebugPrint("PAUSE_MANUAL");
            break;
        case STOP_MANUAL:
            DebugPrint("STOP_MANUAL");
            break;
        case RUN_AUTO:
            DebugPrint("RUN_AUTO");
            break;
        case PAUSE_AUTO:
            DebugPrint("PAUSE_AUTO");
            break;
        case STOP_AUTO:
            DebugPrint("STOP_AUTO");
            break;
        case FLEXION_AUTO:
            DebugPrint("FLEXION_AUTO");
            break;
        case TOLERANCE_TESTING:
            DebugPrint("TOLERANCE_TESTING");
            break;
        case CALIBRATING:
            DebugPrint("CALIBRATING");
            break;
    }
}

//Function: PrintU8
//Inputs:   UINT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a number out the debug UART. String must be null-terminated.
//Example: PrintU( 1234567 ); results in "01234567".
void PrintU8( UINT16 val )
{
    char valStr[9];

    if (TRUE == debug.enabled)
    {
        usitoa8( val, valStr );
        SendUART2( valStr, sizeof(valStr)-1 );   //Don't send null termination
    }
}

//Function: PrintUs
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          UINT8 val - value to be printed
//Outputs: none
//
//Description:
//Prints a short string and number out the debug UART. String must be
//null-terminated. Example: PrintUs( "Value=", 158 ); results in "Value=349".
void PrintUs( const char *str, UINT8 val )
{
    char valStr[] = "0000";

    if (TRUE == debug.enabled)
    {
        DebugPrint( str );
        usitoa4( val, valStr );
        SendUART2( &valStr[1], sizeof(valStr)-2 );  //Don't send null termination
    }
}

//Function: PrintU3
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          UINT8 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and 3-digit number out the debug UART. String must be
//null-terminated. Example: PrintUs( "Value=", 575 ); results in "Value=575".
void PrintU3( const char *str, UINT16 val )
{
    char valStr[] = "0000";

    if (TRUE == debug.enabled)
    {
        DebugPrint( str );
        usitoa4( val, valStr );
        SendUART2( &valStr[1], sizeof(valStr)-2 );  //Don't send null termination
    }
}

//Function: PrintI
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          INT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and number out the debug UART. String must be null-terminated.
//Example: PrintI( "Value=", 349 ); results in  "Value= 00349".
//Example: PrintI( "Value=", -349 ); results in "Value=-00349".
void PrintI( const char *str, INT16 val )
{
    char valStr[] = " 00000";
    UINT16 uval;

    if (TRUE == debug.enabled)
    {
        DebugPrint( str );

        if (val < 0)
        {
            valStr[0] = '-';
            uval = (UINT16)(0-val);
        }
        else
        {
            uval = (UINT16)val;
        }
        usitoa5( uval, &valStr[1] );
        SendUART2( valStr, sizeof(valStr)-1 );   //Don't send null termination
    }
}

//Function: PrintIs
//Inputs:   char *str - pointer to data to send; must be null-terminated
//          INT16 val - value to be printed
//Outputs: none
//
//Description:
//Prints a string and number out the debug UART. String must be null-terminated.
//Example: PrintI( "Value=", 349 ); results in  "Value= 349".
//Example: PrintI( "Value=", -349 ); results in "Value=-349".
void PrintIs( const char *str, INT16 val )
{
    char valStr[] = " 000";
    UINT16 uval;

    if (TRUE == debug.enabled)
    {
        DebugPrint( str );

        if (val < 0)
        {
            uval = (UINT16)(0-val);
        }
        else
        {
            uval = (UINT16)val;
        }
        usitoa4( uval, &valStr[0] );
        if (val < 0)
        {
            valStr[0] = '-';
        }
        else
        {
            valStr[0] = ' ';
        }
        SendUART2( valStr, sizeof(valStr)-1 );   //Don't send null termination
    }
}

void TestDebugging( void )
{
    static char string[] = "Testing Debugging...\r\n";
    DebugPrint(string);
    PrintU8(65535);
    PrintUs("UINT8: ", 158);
    PrintU("UINT16", 349);
    PrintU3("UINT16: ", 349);
    PrintI("INT16: ", -349);
    PrintIs("INT16: ", -349);
    DebugPrint("\n\n\n\r");
}

//Function: HandleDebugging
//Inputs:   none
//Outputs:  none
//
//Description:
//Get inputs from UART and parses them, performing debug functions as requested.
//Call in main loop repeatedly.
void HandleDebugging( void )
{
    UINT8 i;
    UINT16 temp16;
    char str[] = "a";       //Must use the "" to get a null character
    char byte;

    static char dbHelp[] =  DBG_HELP;
    static char dbEn[] =    DBG_ENABLE;
    static char dbBOn[] =   DBG_BON;
    static char dbBOff[] =  DBG_BOFF;
    static char dbBml[] =   DBG_BML;
    static char dbYa1[] = DBG_YA1;
    static char dbYa2[] = DBG_YA2;
    static char dbCsh[] = DBG_CSH;
    static char dbSl1[] = DBG_SL1;
    static char dbSl2[] = DBG_SL2;
    static char dbYps[] = DBG_YPS;
    static char dbCps[] = DBG_CPS;
    static char dbTps[] = DBG_TPS;
    static char dbLfa[] = DBG_LFA;
    static char dbFla[] = DBG_FLA;
    static char dbSp1[] = DBG_SP1;
    static char dbSp2[] = DBG_SP2;
    static char dbThi[] = DBG_THI;
    static char dbTmi[] = DBG_TMI;
    static char dbDmi[] = DBG_DMI;
    static char dbTts[] = DBG_TTS;
    static char dbTdt[] = DBG_TDT;
    static char dbTfa[] = DBG_TFA;
    static char dbTfr[] = DBG_TFR;
    static char dbTsr[] = DBG_TSR;
    

    if (TRUE == debug.blink)
    {
        if (GetUptime() > debug.blinkMs)
        {
            DEBUGLED ^= 1;
            //DebugPrint("Flash DBG\r\n");
            debug.blinkMs = GetUptime() + BLINK_RATE_MS;
        }
    }

    if (GetNbytesReadyUART2() > 0)   //Received a byte via UART
    {
        (void)ReadUART2( str, 1);    //Get the byte
        if (DBG_ESCAPE != str[0])
        {
            DebugPrint( str );          //Echo the byte
        }
        byte = str[0];

        switch (debug.state)
        {
            case DBG_RDY:
                //Received a character: is it a valid first character?
                debug.nRcv = 1;                             //1st byte received
                debug.rcv[0] = byte;                        //Save the data
                debug.state = DBG_RCV;                      //Receiving data
                switch (byte)
                {
                    case DBG_HELP1:
                        debug.nExp = sizeof(dbHelp);    //Size includes \0
                        
                        break;
                    case DBG_ENABLE1:
                        debug.nExp = sizeof(dbEn);      //Size includes \0
                        break;
                    case DBG_BON1:
                        debug.nExp = sizeof(dbBOn);     //Size includes \0
                        break;
                    case DBG_P:
                        debug.nExp = sizeof(dbYa1);
                        break;
                    default:
                        debug.nRcv = 0;                 //1st byte invalid
                        debug.state = DBG_RDY;          //Restart
                        break;
                }   //switch (byte)
                break;      //DBG_RDY
            case DBG_RCV:
                debug.rcv[debug.nRcv++] = byte;
                if ((debug.nRcv > debug.nExp) || (debug.nRcv >= DBG_NCHAR))
                {
                    debug.nRcv = 0;
                    debug.state = DBG_RDY;      //Too many received; restart
                    DebugPrint( "\r\nErr\r\n" );
                }
                else if (debug.nRcv == debug.nExp)
                {
                    //Check if a return was received (\r or \n)
                    if (('\r' == byte) || ('\n' == byte))
                    {
                        DebugPrint( "\n" );
                        //Check for a help message
                        if (debug.rcv[0] == dbHelp[0])
                        {
                            i = debug.enabled;    //Save status
                            debug.enabled = TRUE; //Enable printing temporarily
                            /*if (TRUE == i)
                            {
                                DebugPrint( "\nDebug ENabled\r\n" );
                            }
                            else
                            {
                                DebugPrint( "\nDebug DISabled\r\n" );
                            }*/
    
                            //DebugPrint( "; HM-1000 Rev " );
                            //DebugPrint( FW_REV );
                           /* for (temp16=0; temp16<50000; temp16++)
                            {
                                Nop();  //Delay to let things get printed
                                Nop();  //Delay to let things get printed
                                Nop();  //Delay to let things get printed
                                Nop();  //Delay to let things get printed
                                Nop();  //Delay to let things get printed
                            }*/


                            DebugPrint( "\r\n\nMenu:\r\n\n" );
                            __delay_ms(5);
                            
                            DebugPrint( "DBEN:   Enable debugging\r\n"      );
                            __delay_ms(5);
                            DebugPrint( "BON/OF: 1-sec blink on/off\r\n"    );
                            __delay_ms(5);
                            DebugPrint( "BML:    Blink in main loop\r\n"    );
                            __delay_ms(5);
                            DebugPrint( "PYA1:   Print y axis force 1\r\n"  );
                            __delay_ms(5);
                            DebugPrint( "PYA2:   Print y axis force 2\r\n"  );
                            __delay_ms(5);
                            DebugPrint( "PCSH:   Print cushion force \r\n"  );
                            __delay_ms(5);
                            DebugPrint( "PSL1:   Print spare load 1\r\n"    );
                            __delay_ms(5);
                            DebugPrint( "PSL2:   Print spare load 2\r\n"    );
                            __delay_ms(5);
                            DebugPrint( "PYPS:   Print Y Position\r\n"      );
                            __delay_ms(5);
                            DebugPrint( "PCPS:   Print Column Position\r\n" );
                            __delay_ms(5);
                            DebugPrint( "PTPS:   Print Tension Position\r\n" );
                            __delay_ms(5);
                            DebugPrint( "PLFA:   Print Lateral Flexion Angle\r\n");
                            __delay_ms(5);
                            DebugPrint( "PFLA:   Print Flexion Angle\r\n");
                            __delay_ms(5);
                            DebugPrint( "PSP1:   Print Spare Position 1\r\n");
                            __delay_ms(5);
                            DebugPrint( "PSP2:   Print Spare Position 2\r\n");
                            __delay_ms(5);
                            DebugPrint( "PTHI:   Print Table Height Motor Current\r\n");
                            __delay_ms(5);
                            DebugPrint( "PDMI:   Print Distraction Motor Current\r\n" );
                            __delay_ms(5);
                            DebugPrint( "PTTS:   Print Target Table State\r\n");
                            __delay_ms(5);
                            DebugPrint( "PTDT:   Print Target Dwell Time\r\n");
                            __delay_ms(5);
                            DebugPrint( "PTFA:   Print Target Flexion Angle\r\n");
                            __delay_ms(5);
                            DebugPrint( "PTFR:   Print Target Force\r\n");
                            __delay_ms(5);
                            DebugPrint( "PTSR:   Print Target Seconds Remaining\r\n");
                            __delay_ms(5);
                            
                            debug.enabled = i;      //Restore status
                        }
                        //Check if debugging is being enabled
                        else if (debug.rcv[0] == dbEn[0])
                        {
                            if (TRUE == cmp( debug.rcv, dbEn, debug.nRcv-1))
                            {
                                debug.enabled = TRUE;
                                DebugPrint( "Debugging enabled\r\n" );
                            }
                        }
                        else if (TRUE == debug.enabled)
                        {
                            //Check the incoming message
                            switch (debug.rcv[0])
                            {
                                case DBG_BON1:
                                    if(TRUE==cmp(debug.rcv,dbBOn,debug.nRcv-1))
                                    {
                                        debug.blink = TRUE;
                                        debug.blinkMl = FALSE;
                                        DebugPrint("Turn on dbg blink\r\n");
                                    }
                                   else if(TRUE==cmp(debug.rcv,dbBOff,debug.nRcv-1))
                                    {
                                        debug.blink = FALSE;
                                        debug.blinkMl = FALSE;
                                        DebugPrint("Turn off dbg blink\r\n");
                                        DEBUGLED = 0;   //Turn it off
                                    }
                                    else if(TRUE==cmp(debug.rcv,dbBml,debug.nRcv-1))
                                    {
                                        debug.blink = FALSE;
                                        debug.blinkMl = TRUE;
                                       DebugPrint("BlnkMnLoop\r\n");
                                        DEBUGLED = 0;   //Turn it off
                                    }
                                    break;
                                case DBG_P:
                                    if(TRUE == cmp(debug.rcv, dbYa1, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Y-Axis Force 1  = ", GetYAxisForce1());
                                        PrintU("  ADC Average  = ", getAdcAvg(YAFORCE1));
                                        PrintU("  ADC Latest  = ", getAdcLatest(YAFORCE1));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbYa2, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Y-Axis Force 2  = ", GetYAxisForce2());
                                        PrintU("  ADC Average  = ", getAdcAvg(YAFORCE2));
                                        PrintU("  ADC Latest  = ", getAdcLatest(YAFORCE2));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbCsh, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Cush Force = ", GetCushionForce());
                                        PrintU("  ADC Average  = ", getAdcAvg(CUSHFORCE));
                                        PrintU("  ADC Latest  = ", getAdcLatest(CUSHFORCE));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbSl1, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Spare Load 1 = ", GetSpareLoad1());
                                        PrintU("  ADC Average  = ", getAdcAvg(SPARELOAD1));
                                        PrintU("  ADC Latest  = ", getAdcLatest(SPARELOAD1));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbSl2, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Spare Load 2 = ", GetSpareLoad2());
                                        PrintU("  ADC Average  = ", getAdcAvg(SPARELOAD2));
                                        PrintU("  ADC Latest  = ", getAdcLatest(SPARELOAD2));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbYps, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Y Position = ", GetYPosition());
                                        PrintU("  ADC Average  = ", getAdcAvg(Y_POSITION));
                                        PrintU("  ADC Latest  = ", getAdcLatest(Y_POSITION));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbCps, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Column Position = ", GetColumnPosition());
                                        PrintU("  ADC Average  = ", getAdcAvg(COL_POSITION));
                                        PrintU("  ADC Latest  = ", getAdcLatest(COL_POSITION));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTps, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Tension Position = ", GetTensionPosition());
                                        PrintU("  ADC Average  = ", getAdcAvg(TEN_POSITION));
                                        PrintU("  ADC Latest  = ", getAdcLatest(TEN_POSITION));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbLfa, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Lateral Flexion Angle = ", GetLateralFlexAngle());
                                        PrintU("  ADC Average  = ", getAdcAvg(LAT_FLEX_ANG));
                                        PrintU("  ADC Latest  = ", getAdcLatest(LAT_FLEX_ANG));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbFla, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Flexion Angle = ", GetFlexionAngle());
                                        PrintU("  ADC Average  = ", getAdcAvg(FLEXION_ANG));
                                        PrintU("  ADC Latest  = ", getAdcLatest(FLEXION_ANG));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbSp1, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Spare Position 1 = ", GetSparePosition1());
                                        PrintU("  ADC Average  = ", getAdcAvg(SPAREPOS1));
                                        PrintU("  ADC Latest  = ", getAdcLatest(SPAREPOS1));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbSp2, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Spare Position 2 = ", GetSparePosition2());
                                        PrintU("  ADC Average  = ", getAdcAvg(SPAREPOS2));
                                        PrintU("  ADC Latest  = ", getAdcLatest(SPAREPOS2));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbThi, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Table Height Motor Current = ", GetTableHeightMotorCurrent());
                                        PrintU("  ADC Average  = ", getAdcAvg(TAB_HEIGHT_ISENSE));
                                        PrintU("  ADC Latest  = ", getAdcLatest(TAB_HEIGHT_ISENSE));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTmi, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Tension Motor Current = ", GetTensionMotorCurrent());
                                        PrintU("  ADC Average  = ", getAdcAvg(TENSION_ISENSE));
                                        PrintU("  ADC Latest  = ", getAdcLatest(TENSION_ISENSE));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbDmi, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Distraction Motor Current = ", GetDistractionMotorCurrent());
                                        PrintU("  ADC Average  = ", getAdcAvg(DISTRACT_ISENSE));
                                        PrintU("  ADC Latest  = ", getAdcLatest(DISTRACT_ISENSE));
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTts, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Target Table State = ", GetTargetTableState());
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTdt, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Target Dwell Time = ", GetTargetDwellTime());
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTfa, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Target Flexion Angle = ", GetTargetFlexionAngle());
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTfr, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Target Force = ", GetTargetForce());
                                        DebugPrint("\r\n");
                                    }
                                    else if(TRUE == cmp(debug.rcv, dbTsr, debug.nRcv-1)){
                                        DebugPrint("\r\n");
                                        PrintU("Target Seconds Remaining = ", GetTargetSecondsRemaining());
                                        DebugPrint("\r\n");
                                    }
                                    break;
                                default:
                                    DebugPrint("\r\nERR\r\n");
                                    break;
                            }
                        }

                        debug.nRcv = 0;
                        debug.state = DBG_RDY;      //Restart
                    }
                }
                else if (('\r' == byte) || ('\n' == byte))
                {
                    //Received an early enter
                    DebugPrint("\r\n");
                    debug.nRcv = 0;
                    debug.state = DBG_RDY;          //Restart
                }
                else
                {
                    //Save the byte and wait
                }
                break;
        }
    }
}

void BlinkMLOn( void )
{
    if ((TRUE == debug.enabled) && (TRUE == debug.blinkMl))
    {
        DEBUGLED = ON;
    }
}

void BlinkMLOff( void )
{
    if ((TRUE == debug.enabled) && (TRUE == debug.blinkMl))
    {
        DEBUGLED = OFF;
    }
}


