/*!
 \file   main.c
 \date   November-2024 (created)
 \brief  main application module
*/
#include "main.h"
#include "scheduler.h"
#include "systick.h"
#include "devicetask.h"

static HidProp_t hidProp;
static HidEndp_t hidEndp;
static RtcEvents_t rtcEvents;
static uint32_t rtcValue;
static bool isRtcDataRequsted = false;
static uint8_t devFamily = DS18B20;
static uint8_t usbRxBuffer[wMaxPacketSize];

/*!
 \brief Program entry point
*/
int main(void)
{
    /* Program startup indicator */
    LED_Init(LED1_PORT, LED1_PIN);
    LED_Blink(LED1_PORT, LED1_PIN, 1000000);
    
    /* Initialize the task scheduler */
    InitTaskSheduler(&BackgroundTask);
        
    /* Initialize the 1-Wire Bus */
    OW_InitBus(USART2);
    
    /* Initialize the RTC hardware */
    rtcEvents.secondEvent = RTC_SecondEvent;
    RTC_Init(&rtcEvents);
    RTC_Set_Irq(RTC_IT_SEC);
    
    /* Initializing the SysTick Timer
    InitSystickTimer(SysTick_Callback);
    StartSystickTimer(SYSTICK_INTERVAL); */
    
    /* Search devices task */
    PutTask(DeviceSearchTask, NULL);
    
    /* Initialize the USB hardware */
    hidEndp.rx_buffer = usbRxBuffer;
    hidEndp.rx_handler = USB_HandleRxData;
    USB_SetHidEndp(&hidEndp);
    hidProp.rx_buffer = usbRxBuffer;
    hidProp.rx_handler = USB_HandleRxData;
    USB_SetHidProp(&hidProp);
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
        
    /* Enable Watchdog */
    #ifndef DEBUG
    InitWatchdog();
    #endif

    __enable_irq();
    
    /* Mailoop */
    while (1)
	{
        RunTaskSheduler();
	}
}

/*!
 \brief Background task
*/
static void BackgroundTask(void)
{
    /* Reload watchdog */
    #ifndef DEBUG
    WatchdogReload(KR_KEY_Reload);
    #endif
    
    /* Send RTC data via USB */
    if (isRtcDataRequsted) {
        isRtcDataRequsted = false;
        rtcValue = RTC_GetCounter();
        USB_SendToHost(eGetRtcData, (uint8_t*)&rtcValue, sizeof(rtcValue));
    }
}

/*!
 \brief RTC Second Event callback function
*/
void RTC_SecondEvent(void)
{
    /* Schedule a task to read a 1-wire devices */
    PutTask(DeviceReadTask, &devFamily);
}

/*!
 \brief USB receive callback function
*/
void USB_HandleRxData(void)
{
    AppLayerPacket_t *rx_packet = (AppLayerPacket_t*)usbRxBuffer;
    
    if (rx_packet->rep_id == eRepId_3)  // Check report ID
    {
        switch (rx_packet->opcode)      // Check command code
        {
            case eOwSearch:
                SetOwDataRequest(true);
                PutTask(DeviceSearchTask, NULL); // Schedule a task to search a 1-wire devices
        		break;
            case eOwReadData:
                devFamily = rx_packet->data[0];
                SetOwDataRequest(true);
        		break;
            case eOwWriteData:
                SetOwDataRequest(false);
                PutTask(DeviceWriteTask, rx_packet->data); // Schedule a task to write at 1-wire devices
        		break;
            case eSyncRtc:
                SetOwDataRequest(false);
                RTC_DisableInterrupt(RTC_IT_SEC);
                rtcValue = *((uint32_t*)rx_packet->data);
                RTC_PresetDateTime(rtcValue);
                RTC_EnableInterrupt(RTC_IT_SEC);
                break;
            case eGetRtcData:
                isRtcDataRequsted = true;
                break;
        	default:
        		break;
        }
    }
}
