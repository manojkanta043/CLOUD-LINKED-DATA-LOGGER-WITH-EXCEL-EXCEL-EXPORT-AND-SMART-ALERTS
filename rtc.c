//rtc.c
#include <LPC21xx.H>
#include "types.h"
#include "rtc_defines.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "rtc.h"

char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};


// RTC Initialization: Configures and enables the RTC
void RTC_Init(void)
{
    /* Configure prescalers */
    PREINT  = PREINT_VAL;
    PREFRAC = PREFRAC_VAL;

    /* Enable RTC without resetting the counters */
    CCR = RTC_ENABLE;
}

void GetRTCTimeInfo(s32 *hr,s32 *mi,s32 *se)
{
	*hr = HOUR;
	*mi = MIN;
	*se = SEC;
	
}

void DisplayRTCTime(u32 hr,u32 mi,u32 se)
{
	CmdLCD(GOTO_LINE2_POS0);
	CharLCD((hr/10)+48);	//10's position value
	CharLCD((hr%10)+48);	//1's position value
	CharLCD(':');
	CharLCD((mi/10)+48);	//10's position value
	CharLCD((mi%10)+48);	//1's position value
	CharLCD(':');
	CharLCD((se/10)+48);	//10's position value
	CharLCD((se%10)+48);	//1's position value
	
}

void GetRTCDateInfo(s32 *dt,s32 *mo,s32 *yr)
{
	*dt = DOM;
	*mo = MONTH;
	*yr = YEAR;
	
}

void DisplayRTCDate(u32 dt,u32 mo,u32 yr)
{
	CmdLCD(GOTO_LINE2_POS0);
	CharLCD((dt/10)+48);	//10's position value
	CharLCD((dt%10)+48);	//1's position value
	CharLCD('/');
	CharLCD((mo/10)+48);	//10's position value
	CharLCD((mo%10)+48);	//1's position value
	CharLCD('/');
	U32LCD(yr);	
	
}

void GetRTCDay(s32 *d)
{
	*d = DOW;	
}

void DisplayRTCDay(u32 d)
{
	CmdLCD(GOTO_LINE1_POS0+10);
	StrLCD(week[d]);
}
// RTC_SetTime: Sets the time and date on the RTC
void SetRTCTimeInfo(u32 hour, u32 min, u32 sec) 
{
  // Set hours
	HOUR = hour;	  
  // Set minutes
	MIN = min;	
	// Set seconds
	SEC = sec;
}

void SetRTCDateInfo(u32 dt,u32 mo,u32 yr)
{
	// Set date
	DOM = dt;	  
  // Set month
	MONTH = mo;	
	// Set year
	YEAR = yr;	
}

void SetRTCDay(u32 day)
{
	DOW = day;
}



