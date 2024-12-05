/*!
 \file   devicetask.c
 \date   November-2024 (created)
 \brief  device task handlers
*/
#include "devicetask.h"
#include "onewire.h"
#include "ds18b20.h"
#include "ds1971.h"
#include "string.h"

#define DEVICE_GANG_SIZE    16 // Maximum number of devices on a 1-wire bus

static OW_Device_t owDevice[DEVICE_GANG_SIZE];
static DS18B20_t ds18B20;
static uint8_t deviceCount = 0;
static uint8_t deviceIndex = 0;
static uint8_t dataBuffer[64];

/*!
 \brief Search for devices on the 1-Wire bus
 \param optional parameter
*/
void DeviceSearchTask(void *prm)
{
    uint8_t i;
    deviceCount = 0;
    
    // Clear properties
    for (i = 0; i < DEVICE_GANG_SIZE; i++)
    {
        owDevice[i].address = 0;
        owDevice[i].connected = false;
    }
    
    // Search devices
    for (i = 0; i < DEVICE_GANG_SIZE; i++)
    {
        owDevice[i].connected = OW_SearchRom((uint8_t*)&owDevice[i].address);
        
        if (!owDevice[i].connected)
            break;
        else {
            deviceCount++;
        }
    }
    
    OW_ClearSearchResult();
    DeviceEnumerate(NULL);
}

/*!
 \brief Enumerates all found devices on the 1-Wire bus
 \param optional parameter
*/
void DeviceEnumerate(void *prm)
{
    uint8_t i;
    
    for (i = 0; i < DEVICE_GANG_SIZE; i++)
    {
        if (owDevice[i].address == 0) break;
        USB_SendToHost(eEnumerate, OW_ROM_SIZE, (uint8_t*)&owDevice[i].address);
    }
    
    USB_SendToHost(eEnumerateDone, 0, NULL);
}

/*!
 \brief Returns the number of devices found on the 1-Wire bus
 \retval number of devices
*/
uint8_t DeviceGetCount(void)
{
    deviceIndex = 0;
    return deviceCount;
}

/*!
 \brief Read devices memory
 \param optional parameter (here is the device family code)
*/
void DeviceReadTask(void *prm)
{
    uint16_t data_size = 0;
    uint8_t my_dev_family = 0;
    uint8_t sel_dev_family = *((uint8_t*)prm);
    
    if ((owDevice[deviceIndex].address != 0) && (deviceIndex < deviceCount))
    {
        my_dev_family = owDevice[deviceIndex].address & FAMILY_CODE_MASK;
        if (owDevice[deviceIndex].connected && (my_dev_family == sel_dev_family))
        {
            OW_Reset();
            OW_MatchRom(((uint8_t*)&owDevice[deviceIndex].address));
            
            switch (sel_dev_family)
            {
            case DS1971:
                memcpy(dataBuffer, (uint8_t*)&owDevice[deviceIndex].address, OW_ROM_SIZE);
                DS1971_ReadEeprom((dataBuffer + OW_ROM_SIZE));
                data_size = OW_ROM_SIZE + DS1971_EEPROM_SIZE;
                break;
            case DS18B20:
                DS18B20_ReadScratchpad(&ds18B20);
                memcpy(dataBuffer, (uint8_t*)&owDevice[deviceIndex].address, OW_ROM_SIZE);
                memcpy((dataBuffer + OW_ROM_SIZE), (uint8_t*)&ds18B20, sizeof(ds18B20));
                data_size = OW_ROM_SIZE + sizeof(ds18B20);
                OW_Reset();
                OW_MatchRom(((uint8_t*)&owDevice[deviceIndex].address));
                DS18B20_Convert();  // start new conversion
                break;
            default:
                memcpy(dataBuffer, (uint8_t*)&owDevice[deviceIndex].address, OW_ROM_SIZE);
                data_size = OW_ROM_SIZE;
                break;
            }
            USB_SendToHost(eReadCmd, data_size, dataBuffer);
        }
        deviceIndex++;
    }
}

/*!
 \brief Write devices memory
 \param optional parameter (here is the device address and data to write)
*/
void DeviceWriteTask(void *prm)
{
    uint64_t address = 0;
    uint8_t *buffer = (uint8_t*)prm;
    memcpy((uint8_t*)&address, buffer, OW_ROM_SIZE);
    uint8_t dev_family = (address & 0xFF);
    uint8_t check_sum = 0;
    
    switch (dev_family)
    {
    	case DS18B20:
            OW_Reset();
            OW_MatchRom((uint8_t*)&address);
            DS18B20_WriteScratchpad(buffer + OW_ROM_SIZE);
            OW_Reset();
            OW_MatchRom((uint8_t*)&address);
            DS18B20_CopyScratchpad();
    		break;
        
    	case DS1971:
            check_sum = OW_CalcChecksum(buffer + OW_ROM_SIZE, DS1971_SCRATCHPAD_SIZE);
            OW_Reset();
            OW_MatchRom((uint8_t*)&address);
            DS1971_WriteScratchpad(buffer + OW_ROM_SIZE);
            OW_Reset();
            OW_MatchRom((uint8_t*)&address);
            DS1971_ReadScratchpad(buffer);
            if (check_sum == OW_CalcChecksum(buffer, DS1971_SCRATCHPAD_SIZE))
            {
                OW_Reset();
                OW_MatchRom((uint8_t*)&address);
                DS1971_CopyScratchpad();
                /* Data line is held high for 10ms by the bus master
                   to provide energy for copying data from the scratchpad to EEPROM */
                Wait_ticks(1500000);
            }
    		break;
        
    	default:
    		break;
    }
}
