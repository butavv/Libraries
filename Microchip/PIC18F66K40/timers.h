/* 
 * File:   timers.h
 * Author: Nick
 *
 * Created on March 22, 2018, 1:41 PM
 */

#ifndef TIMERS_H
#define	TIMERS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    typedef enum 
    {
        COUNTDOWN_RUNNING=0,
        COUNTDOWN_PAUSED,
        COUNTDOWN_IDLE
    }countdownState_t;

extern void initTimer0();
extern void initTimer1();
extern void initTimer2();
extern void initTimer3();
extern void initWdt();
extern void initAllTimers();
extern void startTimer1();
extern void stopTimer1();
extern void startTimer0();
extern void stopTimer0();
extern void startTimer2();
extern void stopTimer2();
extern void startTimer3();
extern void stopTimer3();
extern UINT64 GetUptime();
extern void UptimeTick();
extern UINT64 GetCountdownTimerSecondsRemaining();
extern countdownState_t GetCountdownTimerState();
extern void HandleCountdownTimer();
extern void StopCountdownTimer();
extern void PauseCountdownTimer();
extern void StartCountdownTimer();
extern void InitCountdownTimer(UINT64 seconds);
extern BOOL GetCountdownTimerTickReady();
extern void ClearCountdownTimerTickReady();
extern void SetCountdownTimerSecondsRemaining(UINT64 seconds);


#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

