
#ifndef ESP01_H
#define ESP01_H

#include "types.h"


/*************************************************
 * ESP-01 Receive Buffer
 *************************************************/

#define ESP_RX_BUFFER_SIZE    200


/*************************************************
 * Alert Definitions
 *************************************************/

#define ALERT_NORMAL          0
#define ALERT_TEMP_HIGH       1
#define ALERT_GAS_HIGH        2
#define ALERT_BOTH_HIGH       3


/*************************************************
 * ESP-01 Wi-Fi Connection
 *************************************************/

void esp01_connectAP(void);


/*************************************************
 * ThingSpeak Temperature Upload
 *
 * Field 1 = Temperature
 *
 * Called every 3 minutes from main.c
 *************************************************/

u8 esp01_sendToThingspeak(u32 temp);


/*************************************************
 * ThingSpeak Gas Leakage Upload
 *
 * Field 2 = Gas value
 *
 * Called when gas >= GasSetPoint
 *************************************************/

u8 esp01_sendGasToThingspeak(u32 gas);


#endif
