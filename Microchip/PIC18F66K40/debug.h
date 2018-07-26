//**********************************************************************
//
//  Haven Innovation
//  HM1000: PT Table Controller
//
//  Compiler: XC8 v1.44
//
//  File: debug.h
//
//  Description:
//  This file contains UART I/O definitions, functions, and protocol information
//  along with UART-based debugging support.
//
//  2018 Phase 1 Engineering, LLC
//
//**********************************************************************

#ifndef __DEBUG_H__
#define	__DEBUG_H__

////////////////////////////////  DEFINITIONS  /////////////////////////////////
#define MAX_STRING      1000          //Longest string allowed in print functions

//#define EE_DBG          1           //If defined, print EE debugging info

//Protocol for receiving debug commands.
#define DBG_NCHAR       8           //Max characters per command

#define DBG_TMARTIME    50          //ms autoreset time in test mode

//Valid commands. EX: "DBBON [enter]" turns on the 1-second LED blinker.
//EX: "DBCH0005 [enter]" sets the number of channels to 5.
//EX: "DBCFG [enter]" tells the interface board to configure and run the system.
#define DBG_HELP        "?"         //"? [enter]" prints the help message
#define DBG_HELP1       '?'
#define DBG_ENABLE      "DBEN"      //Enable debugging functions
#define DBG_ENABLE1     'D'
#define DBG_BON         "BON"       //Turn on debug LED blink 1sec
#define DBG_BON1        'B'
#define DBG_BOFF        "BOF"       //Turn off debug LED blink 1sec
#define DBG_BML         "BML"       //Blink debug LED once per main loop cycle
#define DBG_YA1         "PYA1"       //Print y axis force 1
#define DBG_YA2         "PYA2"       //Print y axis force 2
#define DBG_CSH         "PCSH"      //Print cush force
#define DBG_SL1         "PSL1"       //Print Spare Load 1
#define DBG_SL2         "PSL2"       //Print Spare Load 2
#define DBG_YPS         "PYPS"        //Print Y Position
#define DBG_CPS         "PCPS"       //Print Column Position
#define DBG_TPS         "PTPS"       //Print Tension Position
#define DBG_LFA         "PLFA"       //Print Lateral flexion angle
#define DBG_FLA         "PFLA"       //Print Flexion Angle
#define DBG_SP1         "PSP1"       //Print Spare Position 1
#define DBG_SP2         "PSP2"       //Print Spare Position 2
#define DBG_THI         "PTHI"       //Print Table Height Motor Current
#define DBG_TMI         "PTMI"       //Print Tension Motor Current
#define DBG_DMI         "PDMI"       //Print Distraction Motor Current
#define DBG_TTS			"PTTS"		//Print Target Table State
#define DBG_TDT			"PTDT"		//Print Target Dwell time
#define DBG_TFA			"PTFA"		//Print Target Flexion Angle
#define DBG_TFR			"PTFR"		//Print Target Force
#define DBG_TSR			"PTSR"		//Print Target Seconds Remaining
#define DBG_P 			'P'
#define DBG_ESCAPE      0x1B        //Escape character

//Toggle debug LED
#define BLINK_RATE_MS   250        //Debug blink rate

extern void InitializeDebugging( void );
extern void DebugPrint( const char* str );
extern void HandleDebugging( void );
extern void PrintI( const char *str, INT16 val );
extern void PrintIs( const char *str, INT16 val );
extern void PrintU( const char *str, UINT16 val );
extern void PrintU8( UINT16 val );
extern void PrintU3( const char *str, UINT16 val );
extern void PrintUs( const char *str, UINT8 val );
extern void BlinkMLOn( void );
extern void BlinkMLOff( void );
extern void TestDebugging( void );
extern void PrintTableState(tableState_t tState);

#endif	//__DEBUG_H__
