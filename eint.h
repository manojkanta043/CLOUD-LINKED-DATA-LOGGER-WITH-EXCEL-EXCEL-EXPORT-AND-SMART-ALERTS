#ifndef _EINT_H_
#define _EINT_H_
#include "types.h"

#define PIN_FUNC2 3
#define EINT2_INPUT_PIN PIN_FUNC2 
#define EINT2_PIN 7//p0.07
#define EINT2_VIC_CHNO 16 //16th channel
void init(void);
void eint2_isr(void) __irq;
#endif
