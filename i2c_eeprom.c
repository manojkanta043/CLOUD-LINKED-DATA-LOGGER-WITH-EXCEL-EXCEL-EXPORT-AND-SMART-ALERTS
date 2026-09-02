//i2c_eeprom.c

#include "types.h"
#include "i2c.h"
#include "delay.h"
#include "i2c_eeprom.h"

/*----------------------------------------------------
 * Function : i2c_eeprom_bytewrite()
 * Purpose  : Write one byte to EEPROM
 *---------------------------------------------------*/
void i2c_eeprom_bytewrite(u8 slaveAddr,u16 wBuffAddr,u8 wByte)
{
    i2c_start();

    /* Slave Address + Write */
    i2c_write(slaveAddr << 1);

    /* EEPROM Address (High Byte) */
    i2c_write((u8)(wBuffAddr >> 8));

    /* EEPROM Address (Low Byte) */
    i2c_write((u8)wBuffAddr);

    /* Data Byte */
    i2c_write(wByte);

    i2c_stop();

    /* EEPROM Internal Write Cycle */
    delay_ms(10);
}

/*----------------------------------------------------
 * Function : i2c_eeprom_randomread()
 * Purpose  : Read one byte from EEPROM
 *---------------------------------------------------*/
u8 i2c_eeprom_randomread(u8 slaveAddr,u16 rBuffAddr)
{
    u8 rByte;

    i2c_start();

    /* Slave Address + Write */
    i2c_write(slaveAddr << 1);

    /* EEPROM Address (High Byte) */
    i2c_write((u8)(rBuffAddr >> 8));

    /* EEPROM Address (Low Byte) */
    i2c_write((u8)rBuffAddr);

    /* Repeated START */
    i2c_restart();

    /* Slave Address + Read */
    i2c_write((slaveAddr << 1) | 1);

    /* Read One Byte and Send NACK */
    rByte = i2c_nack();

    i2c_stop();

    return rByte;
}

/*----------------------------------------------------
 * Function : i2c_eeprom_pagewrite()
 * Purpose  : Write multiple bytes
 * Note     : Maximum 64 bytes per page
 *---------------------------------------------------*/
void i2c_eeprom_pagewrite(u8 slaveAddr,u16 wBuffStartAddr,u8 *pBytes,u8 nBytes)
{
    u32 i;

    i2c_start();

    /* Slave Address + Write */
    i2c_write(slaveAddr << 1);

    /* EEPROM Address (High Byte) */
    i2c_write((u8)(wBuffStartAddr >> 8));

    /* EEPROM Address (Low Byte) */
    i2c_write((u8)wBuffStartAddr);

    /* Data Bytes */
    for(i = 0; i < nBytes; i++)
    {
        i2c_write(pBytes[i]);
    }

    i2c_stop();

    /* EEPROM Write Cycle */
    delay_ms(10);
}

/*----------------------------------------------------
 * Function : i2c_eeprom_sequentialread()
 * Purpose  : Read multiple bytes
 *---------------------------------------------------*/
void i2c_eeprom_sequentialread(u8 slaveAddr,u16 rBuffStartAddr,u8 *pBytes,u8 nBytes)
{
    u32 i;

    i2c_start();

    /* Slave Address + Write */
    i2c_write(slaveAddr << 1);

    /* EEPROM Address (High Byte) */
    i2c_write((u8)(rBuffStartAddr >> 8));

    /* EEPROM Address (Low Byte) */
    i2c_write((u8)rBuffStartAddr);

    /* Repeated START */
    i2c_restart();

    /* Slave Address + Read */
    i2c_write((slaveAddr << 1) | 1);

    /* Read nBytes-1 with ACK */
    for(i = 0; i < (nBytes - 1); i++)
    {
        pBytes[i] = i2c_mack();
    }

    /* Read Last Byte with NACK */
    pBytes[i] = i2c_nack();

    i2c_stop();
}
