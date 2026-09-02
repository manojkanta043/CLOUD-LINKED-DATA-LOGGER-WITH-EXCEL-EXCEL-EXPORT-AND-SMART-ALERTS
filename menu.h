#ifndef _MENU_H_
#define _MENU_H_

#include "types.h"

extern u32 TempSetPoint;
extern u32 GasSetPoint;

void Menu(void);
void EditTime(void);
void EditDate(void);
void EditTempSetPoint(void);
void EditGasSetPoint(void);

#endif
