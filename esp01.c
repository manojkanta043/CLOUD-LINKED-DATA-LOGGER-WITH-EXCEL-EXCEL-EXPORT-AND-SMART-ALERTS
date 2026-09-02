//esp01.c
#include <LPC21xx.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "uart.h"
#include "delay.h"
#include "lcd.h"
#include "esp01.h"


/*************************************************
 * Wi-Fi Configuration
 *************************************************/

#define WIFI_SSID       "Manoj043"
#define WIFI_PASSWORD   "manoj@123"


/*************************************************
 * ThingSpeak Configuration
 *************************************************/

#define THINGSPEAK_API_KEY    "9BKKHL6RAF934VH9"
#define THINGSPEAK_HOST       "api.thingspeak.com"
#define THINGSPEAK_PORT       80


/*************************************************
 * ESP-01 Receive Buffer
 *************************************************/

static s8 buff[ESP_RX_BUFFER_SIZE];


/*************************************************
 * Clear ESP Receive Buffer
 *************************************************/

static void ESP_ClearBuffer(void)
{
    u32 j;

    for(j = 0; j < ESP_RX_BUFFER_SIZE; j++)
    {
        buff[j] = '\0';
    }
}


/*************************************************
 * Read ESP-01 Response
 *************************************************/

static u32 ESP_ReadResponse(u32 timeout_ms)
{
    ESP_ClearBuffer();

    return U0_ReadResponse(buff,ESP_RX_BUFFER_SIZE,timeout_ms);
}


/*************************************************
 * Check ESP Response
 *************************************************/

static u8 ESP_ResponseContains(const char *text)
{
    if(strstr((const char *)buff, text) != 0)
    {
        return 1;
    }

    return 0;
}


/*************************************************
 * Send AT Command
 *************************************************/

static u8 ESP_SendCommand(const char *command,const char *expected,u32 timeout_ms)
{
    U0_TxStr((const s8 *)command);

    ESP_ReadResponse(timeout_ms);

    if(ESP_ResponseContains(expected))
    {
        return 1;
    }

    return 0;
}


/*************************************************
 * ESP-01 Wi-Fi Connection
 *************************************************/

void esp01_connectAP(void)
{
    /*************************************************
     * AT
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("ESP01: AT");

    if(ESP_SendCommand("AT\r\n","OK",3000) == 0)
    {
        CmdLCD(0xC0);
        StrLCD("AT ERROR");
        return;
    }


    /*************************************************
     * Disable Echo
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("ESP01: ATE0");

    if(ESP_SendCommand("ATE0\r\n","OK",3000) == 0)
    {
        CmdLCD(0xC0);
        StrLCD("ATE0 ERROR");
        return;
    }


    /*************************************************
     * Set Station Mode
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("ESP01: MODE");

    if(ESP_SendCommand("AT+CWMODE=1\r\n","OK",3000) == 0)
    {
        CmdLCD(0xC0);
        StrLCD("MODE ERROR");
        return;
    }


    /*************************************************
     * Single Connection
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("ESP01: MUX");

    if(ESP_SendCommand("AT+CIPMUX=0\r\n","OK",3000) == 0)
    {
        CmdLCD(0xC0);
        StrLCD("MUX ERROR");
        return;
    }


    /*************************************************
     * Disconnect Existing Wi-Fi
     *************************************************/

    U0_TxStr((const s8 *)"AT+CWQAP\r\n");

    ESP_ReadResponse(3000);

    delay_ms(1000);


    /*************************************************
     * Connect to Wi-Fi
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("CONNECTING WIFI");

    U0_TxStr((const s8 *)"AT+CWJAP=\"Manoj043\",\"manoj@123\"\r\n");

    ESP_ReadResponse(15000);


    /*************************************************
     * Check Wi-Fi Connection
     *************************************************/

    if(ESP_ResponseContains("WIFI GOT IP") ||ESP_ResponseContains("OK"))
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("WIFI CONNECTED");
        delay_ms(1000);
    }
    else
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("WIFI ERROR");
        delay_ms(2000);
    }
}


/*************************************************
 * Open TCP Connection to ThingSpeak
 *************************************************/

static u8 ESP_OpenThingSpeak(void)
{
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("CONNECT THINGS");
	
    U0_TxStr((const s8 *)"AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
	
    ESP_ReadResponse(10000);


    if(ESP_ResponseContains("CONNECT") || ESP_ResponseContains("ALREADY CONNECTED"))
    {
        return 1;
    }


    return 0;
}


/*************************************************
 * Send HTTP Request
 *************************************************/

static u8 ESP_SendHTTP(const char *request)
{
    char cmd[30];
    u32 length;


    /*************************************************
     * Calculate HTTP request length
     *************************************************/

    length = strlen(request);


    /*************************************************
     * Create CIPSEND command
     *************************************************/

    sprintf(cmd,"AT+CIPSEND=%u\r\n",length);


    /*************************************************
     * Send CIPSEND
     *************************************************/

    U0_TxStr((const s8 *)cmd);

    ESP_ReadResponse(5000);


    /*************************************************
     * ESP should return prompt
     *************************************************/

    if(!ESP_ResponseContains(">"))
    {
        return 0;
    }


    /*************************************************
     * Send HTTP GET request
     *************************************************/

    U0_TxStr((const s8 *)request);


    /*************************************************
     * Read SEND OK / response
     *************************************************/

    ESP_ReadResponse(10000);


    if(ESP_ResponseContains("SEND OK"))
    {
        return 1;
    }


    return 0;
}


/*************************************************
 * Temperature Upload
 *
 * ThingSpeak Field 1
 *************************************************/

u8 esp01_sendToThingspeak(u32 temp)
{
    char request[120];


    /*************************************************
     * Open ThingSpeak TCP connection
     *************************************************/

    if(ESP_OpenThingSpeak() == 0)
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("TCP ERROR");

        delay_ms(1000);

        return 0;
    }


    /*************************************************
     * Create HTTP GET request
     *
     * Field 1 = Temperature
     *************************************************/

    sprintf(request,"GET /update?api_key=%s&field1=%lu\r\n",THINGSPEAK_API_KEY,temp);


    /*************************************************
     * Send request
     *************************************************/

    if(ESP_SendHTTP(request))
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("TEMP UPDATED");

        delay_ms(1000);

        return 1;
    }


    /*************************************************
     * Upload failed
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("TEMP ERROR");

    delay_ms(1000);

    return 0;
}


/*************************************************
 * Gas Leakage Upload
 *
 * ThingSpeak Field 2
 *************************************************/

u8 esp01_sendGasToThingspeak(u32 gas)
{
    char request[120];


    /*************************************************
     * Open ThingSpeak TCP connection
     *************************************************/

    if(ESP_OpenThingSpeak() == 0)
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("TCP ERROR");

        delay_ms(1000);

        return 0;
    }


    /*************************************************
     * Create HTTP GET request
     *
     * Field 2 = Gas
     *************************************************/

    sprintf(request,"GET /update?api_key=%s&field2=%lu\r\n",THINGSPEAK_API_KEY,gas);


    /*************************************************
     * Send request
     *************************************************/

    if(ESP_SendHTTP(request))
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("GAS UPDATED");

        delay_ms(1000);

        return 1;
    }


    /*************************************************
     * Upload failed
     *************************************************/

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("GAS ERROR");

    delay_ms(1000);

    return 0;
}

