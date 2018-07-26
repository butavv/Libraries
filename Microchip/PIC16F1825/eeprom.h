/* 
 * File:   eeprom.h
 * Author: Nick Schrock
 *
 * Created on July 16, 2018, 10:46 AM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    extern void WriteEEPROM(UINT16 address, UINT8 data);
    extern UINT16 ReadEEPROM(UINT16 address);
    extern UINT8 ReadEEPROMLow(UINT16 address);
    extern UINT8 ReadEEPROMHigh(UINT16 address);
    extern UINT8 ReadUserIdLsb_1();
    extern UINT8 ReadUserIdLsb_2();
    extern UINT8 ReadUserIdLsb_3();
    extern UINT8 ReadUserIdMsb();
    
    

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

