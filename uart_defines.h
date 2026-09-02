#ifndef UART_DEFINES_H
#define UART_DEFINES_H


/*************************************************
 * LPC2148 CLOCK CONFIGURATION
 *
 * Crystal = 12 MHz
 * CCLK    = 60 MHz
 * PCLK    = 15 MHz
 *************************************************/

#define FOSC        12000000UL
#define CCLK        (FOSC * 5UL)
#define PCLK        (CCLK / 4UL)


/*************************************************
 * UART CONFIGURATION
 *************************************************/

#define BAUD        9600UL

#define DIVISOR     (PCLK / (16UL * BAUD))


/*************************************************
 * UART0 PIN CONFIGURATION
 *
 * P0.0 -> TXD0
 * P0.1 -> RXD0
 *************************************************/

#define PIN_FUN2            1

#define TXD0_PIN_0_0        PIN_FUN2
#define RXD0_PIN_0_1        PIN_FUN2


/*************************************************
 * UART0 LCR
 *************************************************/

#define _8BIT               3
#define WORD_LEN            _8BIT

#define DLAB_BIT            7


/*************************************************
 * UART0 LSR
 *************************************************/

#define DR_BIT              0
#define THRE_BIT            5
#define TEMT_BIT            6


/*************************************************
 * UART INTERRUPTS
 *
 * Polling is used for ESP-01 communication.
 *************************************************/

#define UART_INTS_ENABLE    0


/*************************************************
 * UART0 IER
 *************************************************/

#define RX0_INT_EN_BIT      0
#define TX0_INT_EN_BIT      1


/*************************************************
 * UART0 IIR
 *************************************************/

#define TXD0_STATUS         2
#define RXD0_STATUS         4


/*************************************************
 * STATUS LEDs
 *************************************************/

#define TXD0_STATUS_LED     5
#define RXD0_STATUS_LED     6


/*************************************************
 * VIC
 *************************************************/

#define UART0_VIC_CHNO      6


#endif

