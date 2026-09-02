#include <LPC21xx.h>
#include <stdlib.h>
#include "types.h"
#include "delay.h"
#include "lcd.h"
#include "adc.h"
#include "lm35.h"
#include "mq2.h"
#include "rtc.h"
#include "menu.h"
#include "eint.h"
#include "kpm.h"
#include "i2c.h"
#include "i2c_eeprom.h"
#include "uart.h"
#include "esp01.h"


/*************************************************
 * Alarm Output
 *************************************************/

#define ALARM_PIN       21


/*************************************************
 * EEPROM
 *************************************************/

#define EEPROM_ADDR     0x50

/*
 * EEPROM Memory Map
 *
 * 0x0000 -> Temperature Set Point
 * 0x0010 -> Gas Set Point
 */

#define TEMP_ADDR       0x0000
#define GAS_ADDR        0x0010


/*************************************************
 * Temperature Limits
 *************************************************/

#define TEMP_MIN        20
#define TEMP_MAX        99


/*************************************************
 * Gas Limits
 *************************************************/

#define GAS_MIN         100
#define GAS_MAX         250


/*************************************************
 * RTC Variables
 *************************************************/

s32 hr, min, sec;
s32 date, month, year;


/*************************************************
 * Sensor Variables
 *************************************************/

u32 temp;
u32 gas;


/*************************************************
 * Alert Status
 *************************************************/

u8 alert;


/*************************************************
 * Set Points
 *************************************************/

extern u32 TempSetPoint;
extern u32 GasSetPoint;


/*************************************************
 * External Interrupt Flag
 *************************************************/

volatile u8 editMode = 0;


/*************************************************
 * ThingSpeak Upload Control
 *************************************************/

/*
 * Temperature:
 * Upload once every 3 minutes.
 */

static u8 lastUploadMin = 255;


/*
 * Gas leakage:
 * 0 = no leakage / ready for new detection
 * 1 = leakage already upl*/

static u8 gasLeakUploaded = 0;


int main(void)
{
    /*************************************************
     * Initialization
     *************************************************/

    InitLCD();

    delay_ms(20);

    RTC_Init();

    Init_ADC();

    InitKPM();

    init_i2c();

    Init_UART0();


    /*************************************************
     * Alarm GPIO
     *************************************************/

    IODIR0 |= (1 << ALARM_PIN);

    IOCLR0 = (1 << ALARM_PIN);


    /*************************************************
     * External Interrupt
     *************************************************/

    init();


    /*************************************************
     * Read Temperature Set Point from EEPROM
     *************************************************/

    TempSetPoint =i2c_eeprom_randomread(EEPROM_ADDR,TEMP_ADDR);

    delay_ms(10);


    /*************************************************
     * Validate Temperature Set Point
     *************************************************/

    if((TempSetPoint < TEMP_MIN) ||(TempSetPoint > TEMP_MAX))
    {
        TempSetPoint = 50;

        i2c_eeprom_bytewrite(EEPROM_ADDR,TEMP_ADDR,TempSetPoint);

        delay_ms(10);
    }


    /*************************************************
     * Read Gas Set Point from EEPROM
     *************************************************/

    GasSetPoint =i2c_eeprom_randomread(EEPROM_ADDR,GAS_ADDR);

    delay_ms(10);


    /*************************************************
     * Validate Gas Set Point
     *************************************************/

    if((GasSetPoint < GAS_MIN) ||(GasSetPoint > GAS_MAX))
    {
        GasSetPoint = 180;

        i2c_eeprom_bytewrite(EEPROM_ADDR,GAS_ADDR,GasSetPoint);

        delay_ms(10);
    }


    /*************************************************
     * ESP-01 Initialization
     *************************************************/

    esp01_connectAP();

    delay_ms(2000);


    /*************************************************
     * Welcome Screen
     *************************************************/

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD("SMART DATALOGGER");

    CmdLCD(0xC0);
    StrLCD("INITIALIZING...");

    delay_ms(1500);

    CmdLCD(0x01);


    /*************************************************
     * Main Loop
     *************************************************/

    while(1)
    {
        /*************************************************
         * Read RTC Date
         *************************************************/

        GetRTCDateInfo(&date,&month,&year);


        /*************************************************
         * Read RTC Time
         *************************************************/

        GetRTCTimeInfo(&hr,&min,&sec);


        /*************************************************
         * Read Temperature
         *************************************************/

        temp = ReadLM35();


        /*************************************************
         * Read Gas Sensor
         *************************************************/

        gas = ReadMQ2();


        /*************************************************
         * LCD Line 1 - Date
         *************************************************/

        CmdLCD(0x80);

        StrLCD("DATE:");

        if(date < 10)
            CharLCD('0');

        U32LCD(date);

        CharLCD('/');

        if(month < 10)
            CharLCD('0');

        U32LCD(month);

        CharLCD('/');

        U32LCD(year);


        /*************************************************
         * LCD Line 2 - Time
         *************************************************/

        CmdLCD(0xC0);

        StrLCD("TIME:");

        if(hr < 10)
            CharLCD('0');

        U32LCD(hr);

        CharLCD(':');

        if(min < 10)
            CharLCD('0');

        U32LCD(min);

        CharLCD(':');

        if(sec < 10)
            CharLCD('0');

        U32LCD(sec);


        /*************************************************
         * LCD Line 3 - Sensors
         *************************************************/

        CmdLCD(0x94);

        StrLCD("T:");

        U32LCD(temp);

        StrLCD("C ");

        StrLCD("G:");

        U32LCD(gas);


        /*************************************************
         * LCD Line 4 - Set Points
         *************************************************/

        CmdLCD(0xD4);

        StrLCD("SW->MENU ");

        StrLCD("T:");

        U32LCD(TempSetPoint);

        StrLCD(" G:");

        U32LCD(GasSetPoint);


        /*************************************************
         * Alarm Control
         *************************************************/

        if((temp >= TempSetPoint) ||(gas >= GasSetPoint))
        {
            IOSET0 = (1 << ALARM_PIN);
        }
        else
        {
            IOCLR0 = (1 << ALARM_PIN);
        }


        /*************************************************
         * Alert Status
         *************************************************/

        alert = ALERT_NORMAL;

        if((temp >= TempSetPoint) &&(gas >= GasSetPoint))
        {
            alert = ALERT_BOTH_HIGH;
        }
        else if(temp >= TempSetPoint)
        {
            alert = ALERT_TEMP_HIGH;
        }
        else if(gas >= GasSetPoint)
        {
            alert = ALERT_GAS_HIGH;
        }


        /*************************************************
         * ThingSpeak Temperature Upload
         *
         * Field 1 = Temperature
         *
         * Upload every 3 minutes.
         *************************************************/

        if((min % 3 == 0) &&(lastUploadMin != min))
        {
            if(esp01_sendToThingspeak(temp))
            {
                lastUploadMin = min;
            }
        }


        /*************************************************
         * ThingSpeak Gas Leakage Upload
         *
         * Field 2 = Gas
         *
         * Upload only once when gas leakage
         * is detected.
         *************************************************/

        if(gas >= GasSetPoint)
        {
            if(gasLeakUploaded == 0)
            {
                if(esp01_sendGasToThingspeak(gas))
                {
                    gasLeakUploaded = 1;
                }
            }
        }
        else
        {
            /*
             * Gas returned below the set point.
             *
             * System is ready to detect the next
             * gas leakage.
             */

            gasLeakUploaded = 0;
        }


        /*************************************************
         * Menu via External Interrupt
         *************************************************/

        if(editMode)
        {
            editMode = 0;

            delay_ms(50);

            CmdLCD(0x01);

            CmdLCD(0x80);
            StrLCD("SWITCH PRESSED");

            CmdLCD(0xC0);
            StrLCD("OPENING MENU...");

            delay_ms(1000);

            Menu();

            CmdLCD(0x01);
        }


        /*************************************************
         * Loop Delay
         *************************************************/

        delay_ms(500);
    }
}

