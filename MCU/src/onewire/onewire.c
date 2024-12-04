/*!
 \file   onewire.c
 \date   November-2024 (created)
 \brief  1-wire abstract device driver
 */
#include "onewire.h"
#include "usart.h"

static USART_TypeDef *OneWireBus;

static bool LastDeviceFlag;
static uint8_t LastDiscrepancy;
static uint8_t ROM_NO[8];

/*!
 \brief Initializes the 1-Wire bus
 \param USARTx - used USART module for 1-Wire bus
 */
void OW_InitBus(USART_TypeDef *USARTx)
{
    OneWireBus = USARTx;
    InitUsart(OneWireBus);
}

/*!
 \brief Set/Reset bit on the 1-Wire bus
 \param bit - may be 0 or 1
 \retval bus state
 */
uint8_t OW_SendReceiveBit(uint8_t bit)
{
    uint8_t txData = (bit == 0) ? 0x00 : 0xFF;
    return (UsartTxRxByte(OneWireBus, txData) == 0xFF) ? 1 : 0;
}

/*!
 \brief Write/Read a byte of data on the 1-Wire bus
 \param byte to Wrire
 \retval Received data byte
 */
uint8_t OW_SendReceiveByte(uint8_t byte)
{
    uint8_t rxByte = 0x00;

    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t txBit = (byte >> i) & 0x01;
        uint8_t rxBit = 0;

        rxBit = OW_SendReceiveBit(txBit);

        rxByte |= (rxBit << i);
    }
    
    return rxByte;
}

/*!
 \brief Performs the RESET function on the 1-Wire bus
 \retval operation status
 */
OW_Status_t OW_Reset(void)
{
    OW_Status_t status = OW_OK;
    uint8_t txByte = 0xF0;
    uint8_t rxByte = 0x00;

    UsartSetBaudrate(OneWireBus, 9600);
    
    rxByte = UsartTxRxByte(OneWireBus, txByte);

    UsartSetBaudrate(OneWireBus, 115200);

    if (rxByte == txByte)
    {
        status = OW_ERROR;
    }

    return status;
}

/*!
 \brief Performs the READ ROM function on the 1-Wire bus
 \param data_dst - pointer to a buffer for copying ROM data
 \retval operation status
 */
OW_Status_t OW_ReadRom(uint8_t *data_dst)
{
    uint8_t i, checkSum;
    
    OW_SendReceiveByte(OW_READ_ROM_CMD);
    
    for (i = 0; i < OW_ROM_SIZE; i++)
    {
        *(data_dst + i) = OW_SendReceiveByte(0xFF);
    }
    
    checkSum = OW_CalcChecksum(data_dst, OW_ROM_SIZE - 1);
    
    if (checkSum != data_dst[OW_ROM_SIZE - 1])
    {
        return OW_ERROR;
    }

    return OW_OK;
}

/*!
 \brief Performs the SKIP ROM function on the 1-Wire bus
 */
void OW_SkipRom(void)
{   
    OW_SendReceiveByte(OW_SKIP_ROM_CMD);
}

/*!
 \brief Performs the MATCH ROM function on the 1-Wire bus
 \param 1-Wire device address
 */
void OW_MatchRom(uint8_t *address)
{   
    OW_SendReceiveByte(OW_MATCH_ROM_CMD);
    
    for (uint8_t i = 0; i < OW_ROM_SIZE; i++)
    {
        OW_SendReceiveByte(address[i]);
    }
}

/*!
 \brief Performs the MATCH ROM function on the 1-Wire bus
 \param pointer to a buffer for writing the found device address
 \retval true if successful, otherwise false
 */
bool OW_SearchRom(uint8_t *address)
{
    uint8_t id_bit_number       = 1,
            last_zero           = 0,
            rom_byte_number     = 0,
            rom_byte_mask       = 1,
            id_bit,
            cmp_id_bit,
            search_direction;
    
    bool search_result = false;
    
    if (!LastDeviceFlag)
    {
        if (OW_Reset() != OW_OK)
        {
            LastDeviceFlag = false;
            LastDiscrepancy = 0;
            return false;
        }
        OW_SendReceiveByte(0xF0);
    }
    
    do
    {
        id_bit = OW_SendReceiveBit(1);       // read data bit on 1-Wire bus
        cmp_id_bit = OW_SendReceiveBit(1);   // read data bit on 1-Wire bus
        
        if ((id_bit == 1) && (cmp_id_bit == 1))
        {
			break;
        }
		else
		{
            if (id_bit != cmp_id_bit)
            {
                search_direction = id_bit; // bit write value for search
            }
            else{
                if (id_bit_number < LastDiscrepancy)
                {
                    search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
                }
                else{
                    search_direction = (id_bit_number == LastDiscrepancy);
                }
                if (search_direction == 0)
				{
					last_zero = id_bit_number;
				}
            }
            if (search_direction == 1)
            {
				ROM_NO[rom_byte_number] |= rom_byte_mask;
            }
			else
            {
				ROM_NO[rom_byte_number] &= ~rom_byte_mask;
            }
            OW_SendReceiveBit(search_direction);   // write data bit to the 1-Wire bus
			id_bit_number++;
			rom_byte_mask <<= 1;
			if (rom_byte_mask == 0)
			{
				rom_byte_number++;
				rom_byte_mask = 1;
			}
        }
    } while(rom_byte_number < 8); // read bytes 0..7
    
    if (!(id_bit_number < 65))
    {
        // Search successful so set LastDiscrepancy, LastDeviceFlag, search_result
        LastDiscrepancy = last_zero;
        // Check for last device
        if (LastDiscrepancy == 0) {
			LastDeviceFlag = true;
        }
		search_result = true;	
    }
    
    if (!search_result || !ROM_NO[0])
	{
		LastDiscrepancy = 0;
        LastDeviceFlag = false;
		search_result = false;
	}
    else
    {
        for (int i = 0; i < 8; i++)
        {
            address[i] = ROM_NO[i];
        }
    }
    
    return search_result;
}

/*!
 \brief Clears the specified variables after searching
 */
void OW_ClearSearchResult(void)
{
    LastDeviceFlag = false;
    LastDiscrepancy = 0;
    
    for (int i = 0; i < 8; i++) {
        ROM_NO[i] = 0;
    }
}

/*!
 \brief Calculates the CRC8 checksum for the specified data
 \param pointer to data buffer for CRC8 calculation
 \param size of data buffer
 \retval CRC8 calculated value
 */
uint8_t OW_CalcChecksum(uint8_t *data, uint8_t len)
{
	uint8_t crc = 0;

	while (len--)
    {
        crc = *(crc8_table + (crc ^ *data++));
	}
	return crc;
}
