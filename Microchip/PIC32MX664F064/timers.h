/* 
 * File:   timers.h
 * Author: Nick
 *
 * Created on May 10, 2018, 11:42 AM
 */

#include "types.h"

#ifndef TIMERS_H
#define	TIMERS_H

#ifdef	__cplusplus
extern "C" {
#endif

    extern void InitTimer1();
    extern void InitTimer2();
    extern void InitTimer4();
    extern void InitWdt();
    extern void ClearWdt();
    extern void StartTimer1();
    extern void StartTimer2();
    extern void StartTimer4();
    extern void StopTimer1();
    extern void StopTimer2();
    extern void StopTimer4();
    extern UINT64 GetUptime();
    extern void SysMsTick();
    extern void HandleTimer4Interrupt();


#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

