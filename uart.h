#ifndef UART_H
#define UART_H

#include "types.h"


/*************************************************
 * UART0 Initialization
 *************************************************/

void Init_UART0(void);


/*************************************************
 * Transmit Functions
 *************************************************/

void U0_TxByte(u8 byte);

void U0_TxStr(const s8 *str);

void U0_TxU32(u32 num);

void U0_TxS32(s32 num);

void U0_TxF32(f32 value,u8 decimalPoints);


/*************************************************
 * Receive Functions
 *************************************************/

/*
 * Blocking receive
 *
 * Waits until one byte is received.
 */
u8 U0_RxByte(void);


/*
 * Receive one byte with timeout
 *
 * Return:
 *
 * 1 = byte received
 * 0 = timeout
 */
u8 U0_RxByteTimeout(u8 *byte,u32 timeout_ms);


/*************************************************
 * Read UART/ESP-01 Response
 *
 * buf       = receive buffer
 * size      = buffer size
 * timeout_ms = maximum silent period
 *
 * Return:
 *
 * Number of received characters.
 *************************************************/

u32 U0_ReadResponse(s8 *buf,u32 size,u32 timeout_ms);


#endif
