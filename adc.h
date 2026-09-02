#ifndef _ADC_H_
#define _ADC_H_

#include "types.h"

/* Initialize ADC */
void Init_ADC(void);

/* Read ADC Channel
 * chNo    : ADC channel number
 * adcDVal : 10-bit ADC digital value (0-1023)
 * eAR     : Equivalent analog voltage (0-3.3V)
 */
void Read_ADC(u8 chNo, u32 *adcDVal, f32 *eAR);

#endif
