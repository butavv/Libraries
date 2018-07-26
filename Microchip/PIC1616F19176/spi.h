/* 
 * File:   spi.h
 * Author: Nick Schrock
 *
 * Created on June 20, 2018, 9:01 AM
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    typedef enum
    {
        NTC1,
        NTC2,
        NTC3
    }spiDevice_t;
    
    typedef enum
    {
        SPI_IDLE,
        SPI_READ1,
        SPI_READ2,
        SPI_DONE
    }spiState_t;

extern void InitializeMasterSpi( void );
extern void StartSpi(spiDevice_t device);
extern INT16 GetSpiData();
extern spiState_t GetSpiState();
extern void HandleSpiInterrupt();

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

