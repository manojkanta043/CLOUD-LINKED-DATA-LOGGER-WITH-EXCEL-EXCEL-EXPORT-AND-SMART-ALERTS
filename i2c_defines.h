#ifndef _I2C_DEFINES_H_
#define _I2C_DEFINES_H_

/*---------------- Pin Function ----------------*/
#define FUNC2           1

#define SCL_PIN_0_2     FUNC2
#define SDA_PIN_0_3     FUNC2

/*---------------- Clock ----------------*/
#define FOSC            12000000UL
#define CCLK            (5 * FOSC)
#define PCLK            (CCLK / 4)

/*---------------- I2C Speed ----------------*/
#define I2C_SPEED       100000UL      //100kHz
#define BITRATE         ((PCLK / I2C_SPEED) / 2)

/*---------------- I2CONSET Bits ----------------*/
#define AA_BIT          2
#define SI_BIT          3
#define STO_BIT         4
#define STA_BIT         5
#define I2EN_BIT        6

/*---------------- I2CONCLR Bits ----------------*/
#define AAC_BIT         2
#define SIC_BIT         3
#define STAC_BIT        5
#define I2ENC_BIT       6

/*---------------- EEPROM ----------------*/
#define I2C_EEPROM_SA   0x50

/*---------------- EEPROM Page Size ----------------*/
#define EEPROM_PAGE_SIZE    64

#endif
