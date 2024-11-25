#include "ds1971.h"

/*!
 */
bool DS1971_ReadScratchpad(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_READ_SCRATCHPAD); // cmd
    OW_SendReceiveByte(0x00);                   // memory address

    for (uint8_t i = 0; i < DS1971_SCRATCHPAD_SIZE; i++)
    {
        data[i] = OW_SendReceiveByte(0xFF);
    }
    
    return true;
}

/*!
 */
void DS1971_ReadEeprom(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_READ_MEMORY); // cmd
    OW_SendReceiveByte(0x00);               // memory address
    
    for (uint8_t i = 0; i < DS1971_EEPROM_SIZE; i++)
    {
        data[i] = OW_SendReceiveByte(0xFF);
    }
}

/*!
 */
void DS1971_WriteScratchpad(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_WRITE_SCRATCHPAD);    // cmd
    OW_SendReceiveByte(0x00);                       // memory address
    
    for (uint8_t i = 0; i < DS1971_SCRATCHPAD_SIZE; i++)
    {
        OW_SendReceiveByte(data[i]);
    }
}

/*!
 */
void DS1971_CopyScratchpad(void)
{
    OW_SendReceiveByte(DS1971_COPY_SCRATCHPAD); // cmd
    OW_SendReceiveByte(DS1971_VALIDATION_KEY);  // validation key
}
