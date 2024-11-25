#include "ds1971.h"

/*!
 */
bool DS1971_ReadScratchpad(uint8_t addr, uint8_t cnt, uint8_t *data)
{
    OW_SendReceiveByte(DS1971_READ_SCRATCHPAD); // cmd
    OW_SendReceiveByte(addr);                   // memory address
    OW_SendReceiveByte(cnt);                    // quantity

    for (uint8_t i = 0; i < cnt; i++)
    {
        data[i] = OW_SendReceiveByte(0xFF);
    }
    
//    uint8_t checkSum = OW_CalcChecksum(data, DS1971_SCRATCHPAD_SIZE);
//    
//    if (checkSum != data[DS1971_SCRATCHPAD_SIZE])
//    {
//        return false;
//    }
    
    return true;
}

/*!
 */
void DS1971_ReadEeprom(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_READ_MEMORY);         // cmd
    OW_SendReceiveByte(0x00);                       // memory address
    OW_SendReceiveByte(DS1971_EEPROM_PAGE_SIZE);    // quantity 32 bytes
    
    for (uint8_t i = 0; i < DS1971_EEPROM_PAGE_SIZE; i++)
    {
        data[i] = OW_SendReceiveByte(0xFF);
    }
}

/*!
 */
void DS1971_WriteScratchpad(uint8_t addr, uint8_t cnt, uint8_t *data)
{
    for (uint8_t i = 0; i < cnt; i++)
    {
        data[i] = OW_SendReceiveByte(0xFF);
    }
}
