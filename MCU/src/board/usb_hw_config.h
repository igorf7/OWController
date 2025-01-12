#ifndef __USB_HW_CONFIG_H
#define __USB_HW_CONFIG_H

#include "stm32f10x.h"
#include "usb_lib.h"

/*Unique Devices IDs register set*/
#define ID1     (0x1FFFF7E8)
#define ID2     (0x1FFFF7EC)
#define ID3     (0x1FFFF7F0)

/* USB external pullup
#define USB_DP_PULLUP_PORT              GPIOA
#define USB_DP_PULLUP_PIN               GPIO_Pin_15
#define RCC_APB2Periph_USB_DP_PULLUP    RCC_APB2Periph_GPIOA */

/* Report ID */
typedef enum
{
    eRepId_0 = (uint8_t)0,
    eRepId_1,
    eRepId_2,
    eRepId_3,
    eRepId_4
} RepId_t;

/* Device Opcodes */
typedef enum
{
    eOwSearch = (uint8_t)1,
    eOwEnumerate,
    eOwEnumerateDone,
    eOwReadData,
    eOwWriteData,
    eGetRtcData,
    eSyncRtc
} AppLayerOpcode_t;

/* Application Layer Packet Structure */
typedef struct
{
    uint8_t rep_id;     // report id
    uint8_t opcode;     // comand code
    uint8_t data_size;  // data field size
    uint8_t data[60];   // data field
} AppLayerPacket_t;

__STATIC_INLINE void Wait_ticks(volatile uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

/* Private functions prototypes */
static void IntToUnicode(uint32_t value , uint8_t *pbuf , uint8_t len);

/* API */
void USB_Interrupts_Config(void);
void Set_USBClock(void);
void Set_System(void);
bool USB_SendToHost(uint8_t opcode, uint8_t *data, int16_t size);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Get_SerialNum(void);
void USB_Cable_Config(FunctionalState NewState);
void RHID_Send(uint8_t *pBuff);
uint8_t RHIDCheckState(void);
#endif  /*__USB_HW_CONFIG_H*/
