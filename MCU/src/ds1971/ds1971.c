/*!
 \file   ds1971.c
 \date   November-2024 (created)
 \brief  DS1971/DS2430A device driver
 */
#include "ds1971.h"

/*!
 \brief Reads DS1971/DS2430A Scratchpad data
 \param pointer to data buffer
 \retval true if successful, otherwise false
 */
bool DS1971_ReadScratchpad(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_READ_SCRATCHPAD); // cmd
    OW_SendReceiveByte(0x00);                   // memory address

    for (uint8_t i = 0; i < DS1971_SCRATCHPAD_SIZE; i++) {
        data[i] = OW_SendReceiveByte(0xFF);
    }
    
    return true;
}

/*!
 \brief Reads DS1971/DS2430A EEPROM data
 \param pointer to data buffer
 */
void DS1971_ReadEeprom(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_READ_MEMORY); // cmd
    OW_SendReceiveByte(0x00);               // memory address
    
    for (uint8_t i = 0; i < DS1971_EEPROM_SIZE; i++) {
        data[i] = OW_SendReceiveByte(0xFF);
    }
}

/*!
 \brief Writes DS1971/DS2430A Scratchpad data
 \param data to write
 */
void DS1971_WriteScratchpad(uint8_t *data)
{
    OW_SendReceiveByte(DS1971_WRITE_SCRATCHPAD);    // cmd
    OW_SendReceiveByte(0x00);                       // memory address
    
    for (uint8_t i = 0; i < DS1971_SCRATCHPAD_SIZE; i++) {
        OW_SendReceiveByte(data[i]);
    }
}

/*!
 \brief Copies the scratchpad into the EEPROM
 */
void DS1971_CopyScratchpad(void)
{
    OW_SendReceiveByte(DS1971_COPY_SCRATCHPAD); // cmd
    OW_SendReceiveByte(DS1971_VALIDATION_KEY);  // validation key
}
