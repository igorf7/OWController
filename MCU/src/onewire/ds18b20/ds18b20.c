#include "ds18b20.h"

/*!
 */
void DS18B20_Convert(void)
{
    OW_SendReceiveByte(OW_CONVERT_CMD);
}

/*!
 */
OW_Status_t DS18B20_ReadScratchpad(DS18B20Mem_t *mem)
{
    uint8_t rxData[DS18B20_SCRATCHPAD_SIZE + 1];
    
    OW_SendReceiveByte(OW_READ_SCRATCHPAD_CMD);

    for (uint8_t i = 0; i < (DS18B20_SCRATCHPAD_SIZE + 1); i++)
    {
        rxData[i] = OW_SendReceiveByte(0xFF);
    }
    
    uint8_t checkSum = OW_CalcChecksum(rxData, DS18B20_SCRATCHPAD_SIZE);
    
    if (checkSum != rxData[DS18B20_SCRATCHPAD_SIZE])
    {
        return OW_ERROR;
    }
    
    mem->config.alarm_high = rxData[2];
    mem->config.alarm_low = rxData[3];
    mem->config.resolution = rxData[4];

    int16_t temperature = (rxData[1] << 8) | rxData[0];
    
    switch (mem->config.resolution)
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

    mem->value = (float)temperature * DS18B20_T_STEP;
    
    return OW_OK;
}

/*!
 */
void DS18B20_WriteScratchpad(DS18B20Mem_t *mem, uint8_t *data)
{
    OW_SendReceiveByte(OW_WRITE_SCRATCHPAD_CMD);
    
    for (uint8_t i = 0; i < 3; i++)
    {
        OW_SendReceiveByte(data[i]);
    }
}
