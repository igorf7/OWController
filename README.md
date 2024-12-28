# OWController
Version 1.1.1
## Description
Controls devices on the 1-Wire bus.\n
The OWController (1-Wire bus controller) project contains two levels:\n
1. The lower level, implemented on the STM32F103 microcontroller (Blue Pill PLus board).\n
2. The upper level is a cross-platform application for displaying data received from the lower level.\n
Data exchange between the lower and upper levels is carried out via USB (Custom HID class device).\n
## Implemented
The following functions have been implemented:\n
Reading data from device memory.\n
Writing data to device memory.\n
Displaying device descriptions.\n
Additionally:\n
Writing received data to a CSV file.\n
Display of date and time received from the MCU RTC.\n
## Supported 1-Wire devices
Supported 1-Wire devices:\n
DS18B20 Temperature Sensor\n
DS1971 EEPROM\n
For all other devices, the contents of the ROM and the device description are displayed.\n
Support for any device can be easily implemented by analogy with the existing implementation.\n
