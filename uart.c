#include <LPC21xx.h>

#include "types.h"
#include "uart.h"
#include "uart_defines.h"
#include "pin_connect_block.h"
#include "delay.h"


/*************************************************
 * Initialize UART0
 *
 * Baud : 9600
 * Data : 8 bit
 * Stop : 1 bit
 * Parity : None
 *
 * RX interrupt is NOT used.
 * ESP-01 communication uses polling.
 *************************************************/

void Init_UART0(void)
{
    /*********************************************
     * UART0 TXD0 -> P0.0
     *********************************************/

    CfgPortPinFunc(0,0,TXD0_PIN_0_0);


    /*********************************************
     * UART0 RXD0 -> P0.1
     *********************************************/

    CfgPortPinFunc(0,1,RXD0_PIN_0_1);


    /*********************************************
     * UART0 Line Control
     *
     * 8 data bits
     * 1 stop bit
     * No parity
     * Enable DLAB
     *********************************************/

    U0LCR =WORD_LEN |(1 << DLAB_BIT);


    /*********************************************
     * Load Baud Rate Divisor
     *********************************************/

    U0DLL =(DIVISOR & 0xFF);

    U0DLM =((DIVISOR >> 8) & 0xFF);


    /*********************************************
     * Disable DLAB
     *********************************************/

    U0LCR = WORD_LEN;


    /*********************************************
     * RX Interrupt NOT enabled
     *
     * UART communication is polling based.
     *********************************************/
}


/*************************************************
 * Transmit One Byte
 *************************************************/

void U0_TxByte(u8 byte)
{
    while(((U0LSR >> THRE_BIT) & 1) == 0)
    {
    }
    U0THR = byte;
}


/*************************************************
 * Receive One Byte - Blocking
 *************************************************/

u8 U0_RxByte(void)
{
    while(((U0LSR >> DR_BIT) & 1) == 0)
    {
    }

    return (u8)U0RBR;
}


/*************************************************
 * Receive One Byte With Timeout
 *
 * Return:
 *
 * 1 = byte received
 * 0 = timeout
 *************************************************/

u8 U0_RxByteTimeout(u8 *byte,u32 timeout_ms)
{
    u32 elapsed;


    elapsed = 0;


    /*********************************************
     * Check Pointer
     *********************************************/

    if(byte == 0)
    {
        return 0;
    }


    /*********************************************
     * Wait For Byte
     *********************************************/

    while(elapsed < timeout_ms)
    {
        if(((U0LSR >> DR_BIT) & 1) != 0)
        {
            *byte = (u8)U0RBR;
            return 1;
        }


        delay_ms(1);

        elapsed++;
    }


    /*********************************************
     * Timeout
     *********************************************/

    return 0;
}


/*************************************************
 * Read ESP-01 Response
 *
 * timeout_ms = maximum silent period.
 *
 * Whenever a character arrives, timeout is
 * restarted.
 *************************************************/

u32 U0_ReadResponse(s8 *buf,u32 size,u32 timeout_ms)
{
    u32 i;
    u32 elapsed;
    u8 ch;
    i = 0;
    elapsed = 0;


    /*********************************************
     * Validate Buffer
     *********************************************/

    if((buf == 0) ||(size == 0))
    {
        return 0;
    }


    /*********************************************
     * Receive Characters
     *********************************************/

    while((i < (size - 1)) &&(elapsed < timeout_ms))
    {
        if(U0_RxByteTimeout(&ch,1) != 0)
        {
            buf[i] = (s8)ch;

            i++;


            /*
             * Restart timeout whenever
             * a byte is received.
             */

            elapsed = 0;
        }
        else
        {
            elapsed++;
        }
    }


    /*********************************************
     * Always Null Terminate
     *********************************************/

    buf[i] = '\0';


    return i;
}


/*************************************************
 * Transmit Null Terminated String
 *************************************************/

void U0_TxStr(const s8 *str)
{
    if(str == 0)
    {
        return;
    }


    while(*str != '\0')
    {
        U0_TxByte((u8)*str);
        str++;
    }
}


/*************************************************
 * Transmit Unsigned 32-bit Integer
 *************************************************/

void U0_TxU32(u32 num)
{
    s32 i;
    u8 digit[10];

    i = 0;


    /*********************************************
     * Zero
     *********************************************/

    if(num == 0)
    {
        U0_TxByte('0');

        return;
    }


    /*********************************************
     * Convert Number To Digits
     *********************************************/

    while(num > 0)
    {
        digit[i] =(u8)((num % 10) + '0');
        i++;
        num /= 10;
    }


    /*********************************************
     * Send Digits In Reverse Order
     *********************************************/

    while(i > 0)
    {
        i--;
        U0_TxByte(digit[i]);
    }
}


/*************************************************
 * Transmit Signed 32-bit Integer
 *************************************************/

void U0_TxS32(s32 num)
{
    if(num < 0)
    {
        U0_TxByte('-');

        num = -num;
    }


    U0_TxU32((u32)num);
}


/*************************************************
 * Transmit Floating Point Value
 *************************************************/

void U0_TxF32( f32 value,u8 decimalPoints)
{
    u32 integerPart;
    u8 i;


    /*********************************************
     * Negative Number
     *********************************************/

    if(value < 0)
    {
        U0_TxByte('-');

        value = -value;
    }


    /*********************************************
     * Integer Part
     *********************************************/

    integerPart = (u32)value;

    U0_TxU32(integerPart);


    /*********************************************
     * No Decimal Required
     *********************************************/

    if(decimalPoints == 0)
    {
        return;
    }

    U0_TxByte('.');


    /*********************************************
     * Decimal Part
     *********************************************/

    value =value - integerPart;


    for(i = 0;i < decimalPoints;i++)
    {
        value *= 10.0f;

        integerPart = (u32)value;

        U0_TxByte((u8)(integerPart + '0'));

        value -= integerPart;
    }
}
