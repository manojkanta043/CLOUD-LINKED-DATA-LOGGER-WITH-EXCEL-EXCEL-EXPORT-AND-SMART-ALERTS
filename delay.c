//delay.c
#include "types.h"
#include "delay.h"
void delay_us(u32 dlyUS)
{
	//XTAL=12MHz,CCLK=60MHz
	dlyUS*=12;//N * ~1us 
	while(dlyUS--);
}

void delay_ms(u32 dlyMS)
{
	//XTAL=12MHz,CCLK=60MHz
	dlyMS*=12000;//N * ~1ms 
	while(dlyMS--);
}

void delay_s(u32 dlyS)
{
	//XTAL=12MHz,CCLK=60MHz
	dlyS*=12000000;//N * ~1s
	while(dlyS--);
}
