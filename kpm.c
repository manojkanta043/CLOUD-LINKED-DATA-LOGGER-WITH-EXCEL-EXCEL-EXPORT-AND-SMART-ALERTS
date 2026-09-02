#include "types.h"
#include <LPC21xx.h>
#include "kpm_defines.h"
#include "lcd.h"
#include "delay.h"
#include "lcd_defines.h"
#include "kpm.h"

const s8 kpmLUT[4][4] =
{
    {'1','2','3','/'},
    {'4','5','6','X'},
    {'7','8','9','-'},
    {'C','0','=','+'}
};

void InitKPM()
{
	//cfg row lines as gpio out
	IODIR1|=15<<ROW0;
	//default all col lines are anyway high
}

u32 ColScan(void)
{
	u32 t;
	t=(((IOPIN1>>COL0)&15)<15)?0:1;
	return t;
}

u32 RowCheck(void)
{
	u32 rNo;
	for(rNo=0;rNo<=3;rNo++)
	{
		//ground every row at a time,starting
		//with 0th row
		IOPIN1=((IOPIN1&~(15<<ROW0))|((~(1<<rNo))<<ROW0));
		if(ColScan()==0)
			break;
	}
	//re-initialize all rows grounded
	IOCLR1=15<<ROW0;
	return rNo;
}

u32 ColCheck(void)
{
	u32 cNo;
	for(cNo=0;cNo<=3;cNo++)
	{
		if(((IOPIN1>>(COL0+cNo))&1)==0)
			break;
	}
	return cNo;
}

	s8 KeyScan(void)
{
    u32 rNo, cNo;
    s8 keyV;

    while(ColScan());          // Wait for key press

    rNo = RowCheck();
    cNo = ColCheck();

    keyV = kpmLUT[rNo][cNo];

    while(ColScan() == 0);     // Wait for key release
    delay_ms(20);              // Debounce

    return keyV;
}

void ReadValue(s8 *ptr)
{
    u32 i = 0;
    s8 keyV;

    CmdLCD(GOTO_LINE2_POS0);

    while(1)
    {
        keyV = KeyScan();

        /* Numeric key */
        if((keyV >= '0') && (keyV <= '9'))
        {
            if(i < 8)
            {
                ptr[i++] = keyV;
                ptr[i] = '\0';

                CharLCD(keyV);
            }
        }

        /* C = Clear entire input */
        else if(keyV == 'C')
        {
            i = 0;
            ptr[0] = '\0';

            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("                ");
            CmdLCD(GOTO_LINE2_POS0);
        }

        else if(keyV == '-')
{
    if(i > 0)
    {
        i--;

        ptr[i] = '\0';

        /* Move cursor to deleted character */
        CmdLCD(GOTO_LINE2_POS0 + i);

        /* Erase deleted character */
        CharLCD(' ');

        /* Move cursor back */
        CmdLCD(GOTO_LINE2_POS0 + i);
    }
}
        /* = = Finish input */
        else if(keyV == '=')
        {
            ptr[i] = '\0';
            break;
        }

        delay_ms(100);
    }
}

s8 KeyDetect(void)
{
    u32 rNo, cNo;
    s8 keyV;

    // No key pressed
    if(ColScan())
        return 0;

    // Identify row and column
    rNo = RowCheck();
    cNo = ColCheck();

    keyV = kpmLUT[rNo][cNo];

    // Wait for key release
    while(ColScan() == 0);
    delay_ms(20);

    return keyV;
}
