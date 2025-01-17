/*!
 \file   ds18b20.c
 \date   November-2024 (created)
 \brief  DS18B20 device driver
 */
#include "ds18b20.h"

uint32_t ui_nan = 0x7FA00000;
float *fp_nan = (float*)&ui_nan;
static uint8_t rxDataBuffer[DS18B20_SCRATCHPAD_SIZE + 1];

/*!
 \brief Starts temperature conversion
 */
void DS18B20_Convert(void)
{
    OW_SendReceiveByte(OW_CONVERT_CMD);
}

/*!
 \brief Reads DS18B20 Scratchpad data
 \param pointer to DS18B20 data structure
 \retval true if successful, otherwise false
 */
bool DS18B20_ReadScratchpad(DS18B20_t *mem)
{
    uint16_t temperature, sign;
    
    OW_SendReceiveByte(DS18B20_READ_SCRATCHPAD);

    for (uint8_t i = 0; i < (DS18B20_SCRATCHPAD_SIZE + 1); i++) {
        rxDataBuffer[i] = OW_SendReceiveByte(0xFF);
    }
    
    uint8_t checkSum = OW_CalcChecksum(rxDataBuffer, DS18B20_SCRATCHPAD_SIZE);
    
    if (checkSum != rxDataBuffer[DS18B20_SCRATCHPAD_SIZE]) {
        mem->value = *fp_nan;
        return false;
    }
    
    temperature = (rxDataBuffer[1] << 8) | rxDataBuffer[0];
    sign = temperature & DS18B20_SIGN_MASK;
    
    if (sign != 0) temperature = (0xFFFF - temperature + 1);
    
    mem->conf.alarm_high = rxDataBuffer[2];
    mem->conf.alarm_low = rxDataBuffer[3];
    mem->conf.resolution = rxDataBuffer[4];
    
    switch (mem->conf.resolution)
    {
        case DS18B20_9_BITS_RESOLUTION:
            temperature &= DS18B20_9_BITS_DATA_MASK;
            break;

        case DS18B20_10_BITS_RESOLUTION:
            temperature &= DS18B20_10_BITS_DATA_MASK;
            break;

        case DS18B20_11_BITS_RESOLUTION:
            temperature &= DS18B20_11_BITS_DATA_MASK;
            break;

        case DS18B20_12_BITS_RESOLUTION:
            temperature &= DS18B20_12_BITS_DATA_MASK;
            break;

        default:
            temperature &= DS18B20_12_BITS_DATA_MASK;
            break;
    }
    
    mem->value = (float)temperature * 0.0625f;
    if (sign != 0) mem->value *= (-1);
    
    return true;
}

/*!
 \brief Writes DS18B20 Scratchpad data
 \param data to write
 */
void DS18B20_WriteScratchpad(uint8_t *data)
{
    OW_SendReceiveByte(DS18B20_WRITE_SCRATCHPAD);
    
    for (uint8_t i = 0; i < 3; i++) {
        OW_SendReceiveByte(data[i]);
    }
}

/*!
 \brief Copies the scratchpad into the EEPROM
 */
void DS18B20_CopyScratchpad(void)
{
    OW_SendReceiveByte(DS18B20_COPY_SCRATCHPAD);
}
