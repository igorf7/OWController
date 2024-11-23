#include "devicetask.h"
#include "onewire.h"
#include "ds18b20.h"
#include "string.h"

#define DEVICE_GANG_SIZE    64 // Maximum number of devices on a 1-wire bus

static OW_Device_t owDevice[DEVICE_GANG_SIZE];
//static DS18B20Mem_t ds18B20Mem[DEVICE_GANG_SIZE];
uint8_t devDataBuff[128];
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
    uint8_t buff[16];
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
                    memcpy(buff, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    USB_SendToHost(eReadCmd, OW_ROM_SIZE, buff);
                    break;
                case DS18B20:
                case DS18S20:
                    isThermometer = true;
                    DS18B20_ReadScratchpad(&ds18B20Mem[i]);
                    memcpy(buff, (uint8_t*)&owDevice[i].address, OW_ROM_SIZE);
                    memcpy((buff + OW_ROM_SIZE), (uint8_t*)&ds18B20Mem[i].value, sizeof(ds18B20Mem[i].value));
                    USB_SendToHost(eReadCmd, OW_ROM_SIZE + sizeof(ds18B20Mem[i].value), buff);
                    break;
                case DS1990A:
                default:
                    USB_SendToHost(eReadCmd, OW_ROM_SIZE, (uint8_t*)&owDevice[i].address);
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
