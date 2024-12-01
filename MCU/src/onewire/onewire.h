/*!
 \file   onewire.h
 \date   November-2024 (created)
 \brief  1-wire abstract device driver header file
 */
#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "stm32f10x.h"
#include "stdbool.h"

// Family code mask
#define FAMILY_CODE_MASK    (uint64_t)0xFF

// 1-Wire family codes
#define DS1990A     0x01
#define DS18S20     0x10 
#define DS1971      0x14
#define DS18B20     0x28

// 1-Wire commands
#define OW_READ_ROM_CMD                0x33
#define OW_MATCH_ROM_CMD               0x55
#define OW_SKIP_ROM_CMD                0xCC
#define OW_SEARCH_ROM_CMD              0xF0
#define OW_CONVERT_CMD                 0x44

#define OW_ROM_SIZE                    8U

typedef enum
{
  OW_OK = 0x00,
  OW_ERROR = !OW_OK
} OW_Status_t;

typedef struct {
    uint64_t address;
    bool     connected;
} OW_Device_t;

static const uint8_t crc8_table[] = {
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

/* API */
void OW_InitBus(USART_TypeDef *USARTx);
uint8_t OW_SendReceiveBit(uint8_t bit);
uint8_t OW_SendReceiveByte(uint8_t byte);
OW_Status_t OW_Reset(void);
OW_Status_t OW_ReadRom(uint8_t *data_dst);
void OW_SkipRom(void);
void OW_MatchRom(uint8_t *address);
bool OW_SearchRom(uint8_t *address);
void OW_ClearSearchResult(void);
uint8_t OW_CalcChecksum(uint8_t *data, uint8_t length);
#endif // __ONEWIRE_H
