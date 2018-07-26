 // Note: Errata for this chip suggests that the following put into a
    //       "powerup" assembly file
    // Instructions for creating powerup.as are in the XC8 Users Guide in
    // the docs directory of the compiler distribution.
    //asm("  BSF NVMCON1, 7");
    // Note that, if anywhere in the code, you need to change NVMCON1<7:6>
    // to anything other than 0b10, be sure to change it back afterwards!
    // In other words:
    // You will have to change NVMCON1bits.NVMREG to access User ID,
    // configuration bits, Rev ID, Device ID or Data EEPROM.
    // If you do any of things, be sure to change it back to 0b10 (!))
    
    
    // There is a special source file named powerup.as in this project!

/******** Begin powerup.as*********************/

#include <xc.inc>
    GLOBAL powerup, start
    PSECT powerup, class=CODE, delta=1, reloc=2
powerup:
    BSF NVMCON1, 7
    GOTO start

/******* End powerup.as**********************/