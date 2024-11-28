#include "devicetask.h"
#include "onewire.h"
#include "ds18b20.h"
#include "ds1971.h"
#include "string.h"

#define DEVICE_GANG_SIZE    32 // Maximum number of devices on a 1-wire bus

static OW_Device_t owDevice[DEVICE_GANG_SIZE];
static DS18B20_t ds18B20;
static uint8_t dataBuffer[64];
static uint8_t deviceCount = 0;
static bool isThermometer = false;

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
        else
            deviceCount++;
    }
    
    OW_ClearSearchResult();
    
    DeviceEnumerate(NULL);
}

/*!
 \brief Enumerates all found devices
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
 \brief Read devices memory
 \param optional parameter (here is the device family code)
*/
void DeviceReadTask(void *prm)
{
    uint8_t dev_family = *((uint8_t*)prm);
    
    isThermometer = false;
    
    for (uint8_t i = 0; i < deviceCount; i++)
    {
        if (owDevice[i].address == 0) break; // no more devices, leave cycle
        
        if (owDevice[i].connected && ((owDevice[i].address & FAMILY_CODE_MASK) == dev_family))
        {
            OW_Reset();
            OW_MatchRom(((uint8_t*)&owDevice[i].address));
            
            switch (dev_family)
            {
                case DS1971:
                    memcpy(dataBuffer, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    DS1971_ReadEeprom((dataBuffer + OW_ROM_SIZE));
                    //*(dataBuffer + OW_ROM_SIZE + DS1971_EEPROM_SIZE) = dev_index;
                    USB_SendToHost(eReadCmd, OW_ROM_SIZE + DS1971_EEPROM_SIZE, dataBuffer);
                    break;
                
//                case DS18S20:
//                    break;
                
                case DS18B20:
                    isThermometer = true;
                    DS18B20_ReadScratchpad(&ds18B20);
                    //ds18B20.conf.index = dev_index;
                    memcpy(dataBuffer, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    memcpy((dataBuffer + OW_ROM_SIZE), (uint8_t*)&ds18B20, sizeof(ds18B20));
                    USB_SendToHost(eReadCmd, OW_ROM_SIZE + sizeof(ds18B20), dataBuffer);
                    /// ShowTemperature(ds18B20.value, dev_index); // for example, output to LCD display
                    break;
                
                default:    // for all other devices show only the address
                    memcpy(dataBuffer, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    //*(dataBuffer + OW_ROM_SIZE) = dev_index;
                    USB_SendToHost(eReadCmd, OW_ROM_SIZE, dataBuffer);
                    continue;
            }
        }
    }
    if (isThermometer)
    {
        OW_Reset();
        OW_SkipRom();
        DS18B20_Convert();  // start new conversion for all thermometers
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
            __nop();
    		break;
        
    	default:
    		break;
    }
}
