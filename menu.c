#include <LPC21xx.h>
#include <stdlib.h>

#include "types.h"
#include "lcd.h"
#include "delay.h"
#include "rtc.h"
#include "kpm.h"
#include "menu.h"


/**************** Limits ****************/

#define TEMP_MIN       20
#define TEMP_MAX       99

#define GAS_MIN    100
#define GAS_MAX    250



/**************** Globals ****************/
u32 TempSetPoint = 50;
u32 GasSetPoint  = 180;
/**************** Private Function ****************/

static u32 ReadNumber(u8 digits);



/**************************************************
 Read numeric value from keypad
**************************************************/

static u32 ReadNumber(u8 digits)
{
    u8 i;
    u8 key;
    u32 value = 0;


    for(i=0;i<digits;i++)
    {

        while(1)
        {

            key = KeyScan();


            if((key >= '0') && (key <= '9'))
            {

                CharLCD(key);


                value = (value * 10) + (key - '0');


                /* wait for key release */

                while(KeyScan() == key);


                delay_ms(30);

                break;
            }
        }
    }


    return value;
}



/**************************************************
 Main Menu
**************************************************/

void Menu(void)
{

    u8 key;
	  CmdLCD(0x01);
    delay_ms(2);

    CmdLCD(0x80);
    StrLCD("1.TIME EDIT");

    CmdLCD(0xC0);
    StrLCD("2.DATE EDIT");

    CmdLCD(0x94);
    StrLCD("3.TEMP 4.GAS");

    CmdLCD(0xD4);
    StrLCD("C.EXIT");


    while(1)
    {

        key = KeyScan();
        switch(key)
        {
        case '1':
        EditTime();
        break;

        case '2':
        EditDate();
        break;

        case '3':
        EditTempSetPoint();
        break;

        case '4':
        EditGasSetPoint();
        break;

        case 'C':
        CmdLCD(0x01);
        StrLCD("EXIT MENU");
        delay_ms(500);
        return;
			}
    delay_ms(50);
		}
}



/**************************************************
 Edit Time
**************************************************/

void EditTime(void)
{

    u32 hr,min,sec;


    CmdLCD(0x01);


    CmdLCD(0x80);
    StrLCD("EDIT TIME");


    CmdLCD(0xC0);
    StrLCD("HH:");

    hr = ReadNumber(2);



    if(hr > 23)
    {

        CmdLCD(0x01);

        StrLCD("INVALID HOUR");

        delay_ms(1000);

        return;

    }



    CmdLCD(0x94);
    StrLCD("MM:");

    min = ReadNumber(2);



    if(min > 59)
    {

        CmdLCD(0x01);

        StrLCD("INVALID MIN");

        delay_ms(1000);

        return;

    }


    CmdLCD(0xD4);
    StrLCD("SS:");

    sec = ReadNumber(2);



    if(sec > 59)
    {

        CmdLCD(0x01);

        StrLCD("INVALID SEC");

        delay_ms(1000);

        return;

    }



    SetRTCTimeInfo(hr,min,sec);


    CmdLCD(0x01);

    StrLCD("TIME UPDATED");

    delay_ms(1000);

}



/**************************************************
 Edit Date
**************************************************/

void EditDate(void)
{

    u32 date,month,year;


    CmdLCD(0x01);


    CmdLCD(0x80);
    StrLCD("EDIT DATE");


    CmdLCD(0xC0);
    StrLCD("DD:");

    date = ReadNumber(2);



    if((date<1)||(date>31))
    {

        CmdLCD(0x01);

        StrLCD("INVALID DATE");

        delay_ms(1000);

        return;

    }



    CmdLCD(0x94);
    StrLCD("MM:");

    month = ReadNumber(2);



    if((month<1)||(month>12))
    {

        CmdLCD(0x01);

        StrLCD("INVALID MONTH");

        delay_ms(1000);

        return;

    }



    CmdLCD(0xD4);
    StrLCD("YYYY:");

    year = ReadNumber(4);



    if((year<2000)||(year>2099))
    {

        CmdLCD(0x01);

        StrLCD("INVALID YEAR");

        delay_ms(1000);

        return;

    }



    SetRTCDateInfo(date,month,year);



    CmdLCD(0x01);

    StrLCD("DATE UPDATED");

    delay_ms(1000);

}



/**************************************************
 Edit Temperature Set Point
**************************************************/

void EditTempSetPoint(void)
{

    u32 temp;



    CmdLCD(0x01);



    CmdLCD(0x80);

    StrLCD("TEMP SET POINT");



    CmdLCD(0xC0);

    StrLCD("CUR:");

    U32LCD(TempSetPoint);



    CmdLCD(0x94);

    StrLCD("NEW:");



    temp = ReadNumber(2);



    if((temp<TEMP_MIN)||(temp>TEMP_MAX))
    {

        CmdLCD(0x01);

        StrLCD("INVALID TEMP");

        delay_ms(1000);

        return;

    }



    TempSetPoint = temp;



    CmdLCD(0x01);

    StrLCD("TEMP UPDATED");


    delay_ms(1000);

}

void EditGasSetPoint(void)
{
    u32 gas;

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD("GAS SET POINT");

    CmdLCD(0xC0);
    StrLCD("CUR:");
    U32LCD(GasSetPoint);

    CmdLCD(0x94);
    StrLCD("NEW:");

    gas = ReadNumber(3);

    if((gas < GAS_MIN) || (gas > GAS_MAX))
    {
        CmdLCD(0x01);
        StrLCD("INVALID GAS");
        delay_ms(1000);
        return;
    }

    GasSetPoint = gas;

    CmdLCD(0x01);
    StrLCD("GAS UPDATED");
    delay_ms(1000);
}
