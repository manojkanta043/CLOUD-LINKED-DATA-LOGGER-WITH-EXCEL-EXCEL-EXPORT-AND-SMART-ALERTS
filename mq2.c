#include "mq2.h"
#include "adc.h"

u32 ReadMQ2(void)
{
    u32 adcVal;
    f32 voltage;

    /* MQ2 connected to AD0.2 (P0.29) */
    Read_ADC(2, &adcVal, &voltage);

    return adcVal;
}
