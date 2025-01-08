# OWController
Version 1.1.1
## Description
Controlling devices on the 1-Wire bus.
The OWController project contains two levels:
1. The lower level, implemented on the STM32F103 microcontroller (Blue Pill PLus board).
   The USART1_TX (PA9) or USART2_TX (PA2) line of the microcontroller is used as a 1-Wire bus.
   A 4.7 kOhm pull-up resistor is required between the line and VCC.
3. The top level is a cross-platform application for displaying data received from lower level devices and configuring device parameters.
Data exchange between the lower and upper levels is carried out via USB (Custom HID Class device).
## Implemented
The following functions have been implemented:\
Reading data from device memory.\
Writing data to device memory.\
Displaying device descriptions.\
Additionally:\
Writing received data to a CSV file.\
Display of date and time received from the MCU RTC.
## Supported 1-Wire devices
Supported 1-Wire devices:\
DS18B20 Temperature Sensor\
DS1971 EEPROM\
For all other devices, the contents of the ROM and the device description are displayed.\
Support for any device can be easily implemented by analogy with the existing implementation.
## Supported OS
Tested on Windows, Linux, Android. \
Qt 6.7.0 is used for building on Android.
