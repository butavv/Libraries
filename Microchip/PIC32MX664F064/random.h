/* 
 * File:   random.h
 * Author: Nick Schrock
 *
 * Created on June 1, 2018, 9:47 AM
 */

#ifndef RANDOM_H
#define	RANDOM_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#define MASK_32 0xB4BCD35C
#define MASK_31 0x7A5BC2E3
#define MAX_VALUE 500
#define MIN_VALUE 0

    extern UINT32 GetRandom( void );
    extern void SeedLFSR( UINT32 seed_1, UINT32 seed_2);
    UINT32 shiftLFSR( UINT32 *lfsr, UINT32 mask);

#ifdef	__cplusplus
}
#endif

#endif	/* RANDOM_H */

