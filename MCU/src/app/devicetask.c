/*!
 \file   devicetask.c
 \date   November-2024 (created)
 \brief  device task handlers
*/
#include "devicetask.h"
#include "onewire.h"
#include "ds18b20.h"
#include "ds1971.h"
#include "rtc.h"
#include "string.h"

#define OW_DEVICE_GANG_SIZE    16 // Maximum number of devices on a 1-wire bus

static OW_Device_t owDevice[OW_DEVICE_GANG_SIZE];
static DS18B20_t ds18B20;
static bool isOwDataRequested = false;
static uint8_t deviceCount = 0;
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
    for (i = 0; i < OW_DEVICE_GANG_SIZE; i++)
    {
        owDevice[i].address = 0;
        owDevice[i].connected = false;
    }
    
    // Search devices
    for (i = 0; i < OW_DEVICE_GANG_SIZE; i++)
    {
        owDevice[i].connected = OW_SearchRom((uint8_t*)&owDevice[i].address);
        
        if (!owDevice[i].connected)
            break;
        else {
            deviceCount++;
        }
    }
    
    OW_ClearSearchResult();
    
    if (isOwDataRequested) {
        DeviceEnumerate(NULL);
    }
}

/*!
 \brief Enumerates all found devices on the 1-Wire bus
 \param optional parameter
*/
void DeviceEnumerate(void *prm)
{
    uint8_t i;
    
    for (i = 0; i < OW_DEVICE_GANG_SIZE; i++)
    {
        if (owDevice[i].address == 0) continue;
        
        USB_SendToHost(eOwEnumerate, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
    }
    
    USB_SendToHost(eOwEnumerateDone, NULL, 0);
    isOwDataRequested = false;
}

/*!
 \brief Returns the number of devices found on the 1-Wire bus
 \retval number of devices
*/
uint8_t DeviceGetCount(void)
{
    return deviceCount;
}

/*!
 \brief Enable/Disable transmit via USB
 \param true - enable transmit, false disable transmit
*/
void SetOwDataRequest(bool state)
{
    isOwDataRequested = state;
}

/*!
 \brief Read devices memory
 \param optional parameter (here is the device family code)
*/
void DeviceReadTask(void *prm)
{
    uint16_t i, data_size;
    uint8_t dev_family = 0;
    uint8_t sel_dev_family = *((uint8_t*)prm);
    
    for (i = 0; i < deviceCount; i++)
    {
        if (owDevice[i].address == 0) continue;
        data_size = 0;
        dev_family = owDevice[i].address & FAMILY_CODE_MASK;
        if (owDevice[i].connected && (dev_family == sel_dev_family)) {
            OW_Reset();
            OW_MatchRom(((uint8_t*)&owDevice[i].address));
            
            switch (dev_family)
            {
                case DS18B20:
                    DS18B20_ReadScratchpad(&ds18B20);
                    memcpy(dataBuffer, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    memcpy((dataBuffer + OW_ROM_SIZE), (uint8_t*)&ds18B20, sizeof(ds18B20));
                    data_size = OW_ROM_SIZE + sizeof(ds18B20);
                    break;
                case DS1971:
                    memcpy(dataBuffer, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    DS1971_ReadEeprom((dataBuffer + OW_ROM_SIZE));
                    data_size = OW_ROM_SIZE + DS1971_EEPROM_SIZE;
                    break;
                default:
                    memcpy(dataBuffer, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    data_size = OW_ROM_SIZE;
                    break;
            }
            if (isOwDataRequested) {
                USB_SendToHost(eOwReadData, dataBuffer, data_size);
            }
        }
    }
    
    isOwDataRequested = false;
    
    if (dev_family == DS18B20) {
        OW_Reset();
        OW_SkipRom();
        DS18B20_Convert();  // start new conversion for al DS18B20
    }
}

/*!
 \brief Write devices memory
 \param optional parameter (here is the device address and data to write)
*/
void DeviceWriteTask(void *prm)
{
    uint64_t address = 0;
    uint8_t check_sum = 0;
    uint8_t *buffer = (uint8_t*)prm;
    
    RTC_DisableInterrupt(RTC_IT_SEC);
    
    memcpy((uint8_t*)&address, buffer, OW_ROM_SIZE);
    
    switch (address & 0xFF) // check device family
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
    RTC_EnableInterrupt(RTC_IT_SEC);
}
