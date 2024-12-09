# OWController (1-Wire controller)
Version 1.0.1
## Description
OWController is a controller for interacting with 1-Wire devices.\
The lower level of OWController is implemented on the STM32F103CB microcontroller (Blue Pill Plus board).\
The TXD line of USART1 or the TXD line of USART2 is used as a 1-Wire bus.\
The upper level of OWController is implemented as a PC program.\
The USB interface (USB Custom HID Device Class) is used for data exchange.
## Contents
MCU — contains the project and source files for the STM32F103CB microcontroller. Keil v5 IDE was used.\
MCU/keil/Binary — contains the firmware file.\
GUI/OWController — contains the project and source files for building the OWController application. Qt Creator IDE and Qt 5.15.2 was used.\
GUI/BIN — contains the executable file of the OWController application for Windows OS.
## Supported devices
![DS18B20](https://github.com/user-attachments/assets/1c758c55-ed8d-4606-a4c6-4ef889a525a8)
 ![DS18B20_conf](https://github.com/user-attachments/assets/a99732c2-abd6-4f2a-8a6a-997084a44d8e)


DS18B20 - measuring and displaying temperature, reading and writing to the configuration register, displaying the device description.

![DS1971_memory](https://github.com/user-attachments/assets/cc3b78bf-cfa1-436e-89a5-44c903f55090)

DS1971/DS2430A - reading, editing and writing to EEPROM memory, displaying the device description.
## Unsupported devices
All other devices can read and display ROM contents, display the device description.\
Support for any device can be easily added by analogy with the existing implementation.
## Additional features
Recording temperature measurements from DS18B20 sensors to a CSV file. The file recording period is set in the program settings.\
Read and display date/time received from MCU RTC. Synchronization with the PC system time occurs automatically at startup.
## P.S.
A 4.7k pull-up resistor is required between PA9 and VCC if USART1 is used as a 1-Wire bus.\
A 4.7k pull-up resistor is required between PA2 and VCC if USART2 is used as a 1-Wire bus.
