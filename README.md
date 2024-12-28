# OWController
Version 1.1.1
## Description
Controls devices on the 1-Wire bus.
The OWController (1-Wire bus controller) project contains two levels:
1. The lower level, implemented on the STM32F103 microcontroller (Blue Pill PLus board).
2. The upper level is a cross-platform (Qt) application for displaying data received from the lower level.
Data exchange between the lower and upper levels is carried out via USB (Custom HID class device).
The application can be built for desktop and mobile platforms.
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
