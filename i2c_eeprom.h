#ifndef _I2C_EEPROM_H_
#define _I2C_EEPROM_H_

#include "types.h"

/* Byte Write */
void i2c_eeprom_bytewrite
(
    u8 slaveAddr,
    u16 wBuffAddr,
    u8 wByte
);

/* Random Read */
u8 i2c_eeprom_randomread
(
    u8 slaveAddr,
    u16 rBuffAddr
);

/* Page Write */
void i2c_eeprom_pagewrite
(
    u8 slaveAddr,
    u16 wBuffStartAddr,
    u8 *pBytes,
    u8 nBytes
);

/* Sequential Read */
void i2c_eeprom_sequentialread
(
    u8 slaveAddr,
    u16 rBuffStartAddr,
    u8 *pBytes,
    u8 nBytes
);

#endif
