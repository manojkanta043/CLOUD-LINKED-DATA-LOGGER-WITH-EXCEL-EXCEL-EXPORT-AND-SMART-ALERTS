#include <LPC21xx.h>
#include "types.h"
#include "pin_connect_block.h"
#include "eint.h"

extern volatile u8 editMode;
volatile u32 IntCount = 0;

void init(void)
{
    // Configure P0.07 as EINT2
    CfgPortPinFunc(0,7,3);

    // Disable EINT2 temporarily
    VICIntEnClr = (1<<EINT2_VIC_CHNO);

    // Clear any pending interrupt
    EXTINT = (1<<2);

    // Edge triggered
    EXTMODE |= (1<<2);

    // Rising edge
    EXTPOLAR |= (1<<2);

    // Configure IRQ mode
    VICIntSelect &= ~(1<<EINT2_VIC_CHNO);

    // Vector slot
    VICVectCntl0 = (1<<5) | EINT2_VIC_CHNO;

    VICVectAddr0 = (u32)eint2_isr;

    // Clear again before enabling
    EXTINT = (1<<2);

    // Enable interrupt
    VICIntEnable |= (1<<EINT2_VIC_CHNO);
}
void eint2_isr(void) __irq
{
    IntCount++;

    editMode = 1;

    EXTINT = (1<<2);

    VICVectAddr = 0;
}
