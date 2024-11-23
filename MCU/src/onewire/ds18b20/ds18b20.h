#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"
#include "onewire.h"

#define DS18B20_9_BITS_RESOLUTION           0x1F
#define DS18B20_10_BITS_RESOLUTION          0x3F
#define DS18B20_11_BITS_RESOLUTION          0x5F
#define DS18B20_12_BITS_RESOLUTION          0x7F

#define DS18B20_9_BITS_DATA_MASK            0x7F8
#define DS18B20_10_BITS_DATA_MASK           0x7FC
#define DS18B20_11_BITS_DATA_MASK           0x7FE
#define DS18B20_12_BITS_DATA_MASK           0x7FF

//#define DS18B20_SIGN_MASK                   0xF800

#define DS18B20_T_STEP                      0.0625f

#define DS18B20_SCRATCHPAD_SIZE             8

typedef struct
{
    uint8_t alarm_high;
    uint8_t alarm_low;
    uint8_t resolution;
} Config_t;

typedef struct
{
    float    value;
    Config_t config;
} DS18B20Mem_t;

/* API */
void DS18B20_Convert(void);
void DS18B20_WriteScratchpad(DS18B20Mem_t *mem, uint8_t *data);
OW_Status_t DS18B20_ReadScratchpad(DS18B20Mem_t *mem);
#endif // __DS18B20_H
