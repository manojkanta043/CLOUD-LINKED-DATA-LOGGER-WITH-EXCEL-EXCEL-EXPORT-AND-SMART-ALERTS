#include "lm35.h"
#include "adc.h"

u32 ReadLM35(void)
{
    u32 adcVal;
    f32 voltage;
    u32 temperature;

    /* LM35 connected to AD0.1 (P0.28) */
    Read_ADC(1, &adcVal, &voltage);

    /* LM35 Output = 10mV/°C */
    temperature = (u32)(voltage * 100.0);

    return temperature;
}
