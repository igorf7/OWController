/*!
 \file   ds1971.h
 \date   November-2024 (created)
 \brief  DS1971/DS2430A device driver header file
 */
#ifndef __DS1971_H
#define __DS1971_H

#include "stm32f10x.h"
#include "onewire.h"

#define DS1971_SCRATCHPAD_SIZE      32U
#define DS1971_EEPROM_SIZE          32U

/* DS18B20 commands */
#define DS1971_WRITE_SCRATCHPAD                     0x0F
#define DS1971_READ_SCRATCHPAD                      0xAA
#define DS1971_COPY_SCRATCHPAD                      0x55
#define DS1971_READ_MEMORY                          0xF0
#define DS1971_WRITE_APPLICATION_REGISTER           0x99
#define DS1971_READ_STATUS_REGISTER                 0x66
#define DS1971_READ_APPLICATION_REGISTER            0xC3
#define DS1971_COPY_ANDLOCK_APPLICATION_REGISTER    0x5A

#define DS1971_VALIDATION_KEY                       0xA5

/* API */
void DS1971_WriteScratchpad(uint8_t *data);
void DS1971_ReadEeprom(uint8_t *data);
bool DS1971_ReadScratchpad(uint8_t *data);
void DS1971_CopyScratchpad(void);
#endif // __DS1971_H
