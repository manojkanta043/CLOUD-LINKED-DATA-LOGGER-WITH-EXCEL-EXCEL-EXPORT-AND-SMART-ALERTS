# Cloud-Linked Data Logger with Excel Export & Smart Alerts

A cloud-connected embedded data logger built around the **LPC2148 ARM7
microcontroller**. The system continuously monitors temperature and gas
conditions, displays real-time information on a 20x4 LCD, stores
configurable set points in EEPROM, uploads temperature data to
ThingSpeak, and sends gas-leakage information to the cloud when a leak
is detected.

## Project Overview

The project is designed for real-time environmental monitoring with
local and cloud-based alerting.

### Main Features

-   Real-time temperature monitoring using **LM35**
-   Gas/smoke monitoring using **MQ-2**
-   Real-time date and time using **RTC**
-   20x4 LCD display for system status and sensor values
-   4x4 keypad for menu navigation and set-point editing
-   External-interrupt switch for opening the configuration menu
-   Temperature and gas set points stored in **AT24C256 EEPROM**
-   Temperature uploaded to **ThingSpeak every 3 minutes**
-   Gas value uploaded when gas leakage crosses the configured threshold
-   Local alarm/buzzer indication when a monitored parameter exceeds its
    set point
-   Configurable temperature and gas thresholds
-   EEPROM retention of set points across power cycles
-   Cloud data suitable for further **Excel/CSV analysis/export**
-   Modular Embedded-C drivers for each peripheral

## System Architecture

``` text
                 +-------------------+
                 |      LM35         |
                 | Temperature Sensor|
                 +---------+---------+
                           |
                           v
+----------+       +-------+--------+       +----------------+
|   MQ-2   |------>|    LPC2148     |<----->|   AT24C256     |
| Gas      |       |    ARM7 MCU    |       |    EEPROM      |
| Sensor   |       +---+---+---+----+       +----------------+
+----------+           |   |   |
                       |   |   +----------------+
                       |   |                    |
                       |   v                    v
                       |  LCD 20x4          4x4 Keypad
                       |
                       +------> RTC
                       |
                       +------> Alarm/Buzzer
                       |
                       v
                  UART Interface
                       |
                       v
                    ESP-01
                       |
                       v
                 Wi-Fi Network
                       |
                       v
                   ThingSpeak
                       |
                       v
                 Excel / CSV
                    Analysis
```
<img width="449" height="275" alt="image" src="https://github.com/user-attachments/assets/2b39accf-2606-4114-b5c0-e79101d5c8af" />

## Hardware Requirements

  -----------------------------------------------------------------------
  Component                           Purpose
  ----------------------------------- -----------------------------------
  **LPC2148**                         Main ARM7 microcontroller

  **LM35**                            Temperature sensing

  **MQ-2 Gas Sensor**                 Gas/smoke leakage detection

  **20x4 LCD**                        Local display

  **4x4 Keypad**                      Menu and set-point input

  **Switch**                          External interrupt / menu trigger

  **ESP-01**                          Wi-Fi communication

  **AT24C256**                        Non-volatile set-point storage

  **DB-9 Cable / USB-UART Converter** UART communication and debugging
  -----------------------------------------------------------------------

## Software Requirements

-   **Keil C Compiler**
-   **Embedded C**
-   **Flash Magic**
-   Proteus (optional, for simulation/testing)
-   ThingSpeak account/channel for cloud monitoring

## Project File Structure

``` text
Cloud-Linked-Data-Logger/
│
├── main.c
│
├── types.h
│
├── delay.c
├── delay.h
│
├── lcd.c
├── lcd.h
├── lcd_defines.h
│
├── kpm.c
├── kpm.h
│
├── adc.c
├── adc.h
│
├── lm35.c
├── lm35.h
│
├── mq2.c
├── mq2.h
│
├── rtc.c
├── rtc.h
│
├── i2c.c
├── i2c.h
├── i2c_defines.h
│
├── i2c_eeprom.c
├── i2c_eeprom.h
│
├── uart.c
├── uart.h
│
├── esp01.c
├── esp01.h
│
├── eint.c
├── eint.h
│
├── menu.c
├── menu.h
│
└── pin_connect_block.h
```

> **Note:** The project has evolved through several module revisions.
> Keep the final `uart.*`, `esp01.*`, keypad/KPM, EEPROM, EINT and
> `main.c` versions together so that the function names used by `main.c`
> match the corresponding headers and source files.

## Module Description

### `main.c`

Implements the complete application logic:

-   Initializes all peripherals
-   Reads RTC date/time
-   Reads LM35 temperature
-   Reads MQ-2 gas value/status
-   Displays sensor data and set points on the LCD
-   Checks temperature and gas thresholds
-   Controls the alarm output
-   Uploads temperature every 3 minutes
-   Uploads gas information when leakage is detected
-   Opens the menu after an external interrupt
-   Uses EEPROM-stored set points

The final application uses the temperature and gas set points stored at
separate EEPROM locations. The project configuration uses a default
temperature set point of **50°C** and a default gas set point of **180**
when invalid EEPROM values are detected.

### `lcd.c / lcd.h / lcd_defines.h`

LCD driver for the 20x4 character LCD.

Used for:

-   Character display
-   String display
-   Integer display
-   RTC display
-   Sensor values
-   Menu screens
-   ESP-01 status/error messages

Example main screen:

``` text
DATE:17/08/17
TIME:15:53:54
T:20C G:169
SW->MENU T:50 G:180
```

### `kpm.c / kpm.h`

4x4 keypad driver used for menu navigation and set-point entry.

The keypad also supports editing/backspace behavior in the value-entry
routine.

### `adc.c / adc.h`

LPC2148 on-chip ADC driver used for analog sensor acquisition.

### `lm35.c / lm35.h`

LM35 temperature sensor driver. The ADC reading is converted into a
temperature value for display and threshold comparison.

### `mq2.c / mq2.h`

MQ-2 gas/smoke sensor interface. The gas level/status is used for
leakage detection and alarm/cloud logging.

### `rtc.c / rtc.h`

RTC driver for maintaining:

-   Date
-   Month
-   Year
-   Hours
-   Minutes
-   Seconds

The RTC values are continuously displayed and can be edited through the
menu.

### `i2c.c / i2c.h / i2c_defines.h`

I2C master driver used to communicate with the external EEPROM.

### `i2c_eeprom.c / i2c_eeprom.h`

AT24C256 EEPROM driver.

The EEPROM is used to retain set-point information even after power
failure.

Example memory map:

``` text
0x0000 -> Temperature Set Point
0x0010 -> Gas Set Point
```

### `uart.c / uart.h`

UART0 driver used for LPC2148-to-ESP-01 serial communication.

The final project uses polling/timeout-based reception rather than
relying on a UART RX interrupt for ESP-01 command-response handling.

### `esp01.c / esp01.h`

ESP-01 Wi-Fi and ThingSpeak communication driver.

Responsibilities include:

-   ESP-01 AT-command communication
-   Wi-Fi connection
-   TCP connection to ThingSpeak
-   Temperature upload
-   Gas-leakage upload
-   Cloud alert/status handling
-   ESP response handling

Alert definitions used by the project include:

``` c
ALERT_NORMAL
ALERT_TEMP_HIGH
ALERT_GAS_HIGH
ALERT_BOTH_HIGH
```

### `eint.c / eint.h`

External interrupt driver used by the configuration switch.

Pressing the switch generates an interrupt and requests the system to
enter the configuration menu.

### `menu.c / menu.h`

Menu interface for local configuration.

Typical options include:

``` text
1. Edit Time
2. Edit Date
3. Edit Temperature Setpoint
4. Edit Gas Setpoint
```

Updated set points are written to EEPROM.

### `delay.c / delay.h`

Software delay routines used throughout the project for LCD timing,
sensor/module timing, menu operation and communication delays.

## Set-Point Logic

The project uses configurable threshold values.

``` text
Temperature:
Minimum = 20°C
Maximum = 99°C
Default = 50°C

Gas:
Minimum = 100
Maximum = 250
Default = 180
```

If an EEPROM value is outside the valid range, the corresponding default
value is restored and written back to EEPROM.

## Cloud Upload Logic

### Temperature

Temperature is uploaded to ThingSpeak once every **3 minutes**.

``` text
Every 3 minutes
       |
       v
Read current temperature
       |
       v
Upload temperature to ThingSpeak
```

The temperature upload is kept separate from the gas-leakage event
upload.

### Gas Leakage

Gas data is uploaded when the measured gas level reaches/exceeds the
configured gas set point.

``` text
Gas >= Gas Set Point
        |
        v
Gas leakage detected
        |
        v
Upload gas value to ThingSpeak
        |
        v
Wait until gas returns below threshold
        |
        v
Ready for next leakage event
```

This prevents repeated uploads for the same continuous leakage event.

## Local Alarm Logic

The alarm output is activated when either monitored parameter reaches
its configured threshold.

``` c
if((temperature >= TempSetPoint) ||
   (gas >= GasSetPoint))
{
    /* Alarm ON */
}
else
{
    /* Alarm OFF */
}
```

Alert status can represent:

``` text
NORMAL
TEMPERATURE HIGH
GAS HIGH
BOTH HIGH
```

## Development / Implementation Procedure

The project follows a modular bring-up approach.

### 1. Create Project Folder

Create a project folder on the PC and keep all source and header files
together.

### 2. Test LCD

Verify:

-   Character constant
-   String constant
-   Integer constant

### 3. Test Keypad

Read keypad values and display them on the LCD.

### 4. Test UART

Transmit strings from LPC2148 and verify them using a serial terminal.

### 5. Test UART Communication

Verify UART communication on hardware and check the required UART
implementation for the ESP-01 interface.

### 6. Test EEPROM

Write `n` bytes to AT24C256 and read the same bytes back.

Verify both:

-   Byte write/read
-   Page write/read, where required

### 7. Test Set-Point Storage

Store the temperature and gas set points in EEPROM and verify that the
values remain available after power cycling.

### 8. Test External Interrupt

Connect the configuration switch to the external interrupt input and
verify an interrupt-driven output response.

### 9. Test LM35

Connect LM35 to the LPC2148 ADC and verify temperature conversion and
LCD display.

### 10. Test MQ-2

Connect the MQ-2 sensor and verify gas/smoke detection and threshold
behavior.

### 11. Test ESP-01

Use a serial terminal/Flash Magic test setup to verify AT-command
responses.

### 12. Connect ESP-01 to LPC2148

Connect the ESP-01 through UART and verify the ESP-01 driver.

### 13. Test ThingSpeak

Send a test value to ThingSpeak before integrating the complete
application.

### 14. Integrate the Main Application

Initialize all modules and run the continuous monitoring loop.

### 15. Long-Duration Test

Verify:

-   Continuous sensor monitoring
-   LCD stability
-   RTC operation
-   EEPROM set-point retention
-   Alarm behavior
-   Wi-Fi connection
-   ThingSpeak temperature upload
-   Gas-leakage cloud upload
-   Menu operation

## Final Runtime Flow

``` text
START
  |
  v
Initialize LPC2148 peripherals
  |
  +--> LCD
  +--> RTC
  +--> ADC
  +--> Keypad
  +--> I2C / EEPROM
  +--> UART
  +--> External Interrupt
  +--> ESP-01
  |
  v
Read stored set points
  |
  v
Connect ESP-01 to Wi-Fi
  |
  v
+--------------------------------------+
|           CONTINUOUS LOOP            |
|                                      |
|  Read RTC                             |
|  Read LM35                            |
|  Read MQ-2                            |
|  Update LCD                           |
|  Compare with set points              |
|  Control alarm                        |
|                                      |
|  Every 3 minutes:                    |
|      Upload temperature              |
|                                      |
|  Gas leakage detected:               |
|      Upload gas value                |
|                                      |
|  Switch interrupt:                   |
|      Open configuration menu          |
|      Edit set point / RTC             |
|      Save set point to EEPROM         |
+--------------------------------------+
  |
  +----> Repeat continuously
```

## LCD Output

The implemented display uses a **20x4 LCD** to show the date, time,
current sensor readings and configured thresholds.

Example:

``` text
DATE:17/08/17
TIME:15:53:54
T:20C G:169
SW->MENU T:50 G:180
```

The shorter text format is intentionally used so the information fits
within the 20-character LCD width.

## Hardware Testing Evidence

The project hardware output includes LCD screenshots showing:

-   RTC date and time
-   Temperature value
-   Gas value
-   Temperature set point
-   Gas set point
-   Menu/switch indication

Add the project photographs to the repository, for example:

``` text
docs/
├── hardware.jpg
├── lcd_output_1.jpg
└── lcd_output_2.jpg
```

Then reference them in this README:

``` markdown
![Hardware Setup](docs/hardware.jpg)

![LCD Output](docs/lcd_output_1.jpg)

![LCD Output](docs/lcd_output_2.jpg)
```

## Cloud Data

ThingSpeak is used as the cloud platform for remote data monitoring.

The channel data can be used for:

-   Temperature trend analysis
-   Gas leakage event tracking
-   Alert history
-   Time-based analysis
-   Excel/CSV export and further processing

**Do not commit Wi-Fi passwords or ThingSpeak API keys to a public
GitHub repository.** Keep credentials in a local configuration file or
replace them with placeholders before pushing the project.

## Expected Result

After successful integration, the system should:

1.  Start the LPC2148 system.
2.  Initialize all required peripherals.
3.  Read temperature and gas values continuously.
4.  Display date, time, sensor values and set points on the 20x4 LCD.
5.  Trigger the alarm when a parameter exceeds its set point.
6.  Upload temperature to ThingSpeak every 3 minutes.
7.  Upload gas information when gas leakage is detected.
8.  Allow local set-point modification through the interrupt switch and
    keypad.
9.  Store updated set points in AT24C256 EEPROM.
10. Continue operating after power cycling with the saved set points.

## Future Scope

-   Remote set-point modification through ThingSpeak/cloud
-   Mobile/web dashboard
-   Additional environmental sensors
-   SMS/e-mail/push notifications
-   Automatic report generation
-   Improved event timestamping and cloud alert history
-   Direct automated Excel/CSV report generation

## Skills & Technologies

``` text
Embedded C
ARM7 / LPC2148
ADC
UART
I2C
RTC
EEPROM
External Interrupts
LCD
Keypad
LM35
MQ-2
ESP8266 / ESP-01
Wi-Fi
ThingSpeak
Keil
Flash Magic
Proteus
```

## Author

**Cloud-Linked Data Logger with Excel Export & Smart Alerts**

Built as an ARM7/LPC2148 embedded IoT project integrating sensor
monitoring, non-volatile configuration storage, Wi-Fi connectivity,
cloud logging and smart alert mechanisms.
