# OWController
Version 1.1.5
## Description
Controlling devices on the 1-Wire bus.
The OWController project contains two levels:
1. The lower level, implemented on the STM32F103 microcontroller (Blue Pill PLus board).
2. The upper level is a cross-platform application for displaying data received from lower level devices and configuring device parameters.
Data exchange between the lower and upper levels is carried out via USB (Custom HID Class device).
## Supported MCU
STM32F103 Series Microcontrollers (ARM Cortex M3).
## Supported OS
Tested on Windows 11, Manjaro Linux, Android 8. \
Qt 6.7.0 is used for building on Android.
Qt 5.15.2 is used for building on Linux and Windows platforms.
## Supported 1-Wire devices
Supported 1-Wire devices:\
DS18B20 Temperature Sensor\
DS1971 EEPROM\
For all other devices, the contents of the ROM and the device description are displayed.\
Support for any device can be easily implemented by analogy with the existing implementation.
## Implemented
The following functions have been implemented:\
Reading data from device memory.\
Writing data to device memory.\
Displaying device descriptions.\
Additionally:\
Writing received data to a CSV file.\
Display of date and time received from the MCU RTC.
## Screenshots
![owc1](https://github.com/user-attachments/assets/7b0ef613-4716-425b-94c8-46ddfcd8e4f9)
![owc2](https://github.com/user-attachments/assets/98e5885f-ff8c-4bb0-a4ff-342a8048ce20)
![owc3](https://github.com/user-attachments/assets/1f720000-0358-47d8-9d02-3c89e4b8e086)\
![owc4](https://github.com/user-attachments/assets/07885023-af41-4f09-abb8-4e9bc3bb78fa)
![owc5](https://github.com/user-attachments/assets/e3fbb657-46d5-4291-8313-61d67e802ae6)
![owc6](https://github.com/user-attachments/assets/d9a73703-e36a-4939-be15-715d8f3e486f)
