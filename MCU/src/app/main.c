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
static uint32_t rtcCounter;
static bool isOnewireReadEnabled = false;
static bool isHeartbeat = false;
static uint8_t devType = DS1971;
static uint8_t usbRxBuffer[wMaxPacketSize];

/*!
 \brief Program entry point
*/
int main(void)
{
    /* Initialize the USB hardware */
    Wait_ticks(1500000); // to sinc with host
    hidEndp.rx_buffer = usbRxBuffer;
    hidEndp.rx_handler = USB_HandleRxData;
    USB_SetHidEndp(&hidEndp);
    hidProp.rx_buffer = usbRxBuffer;
    hidProp.rx_handler = USB_HandleRxData;
    USB_SetHidProp(&hidProp);
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
    
    /* Program startup indicator */
    LED_Init(LED1_PORT, LED1_PIN);
    LED_Blink(LED1_PORT, LED1_PIN, 1000000);
    
    /* Initialize the task scheduler */
    InitTaskSheduler(&BackgroundTask);
        
    /* Initialize the 1-Wire Bus */
    OW_InitBus(USART2);
    
    /* Initialize the RTC hardware */
    rtcEvents.secondEvent = RTC_SecondEvent;
    rtc_init(&rtcEvents);
    rtc_set_irq(RTC_IT_SEC);
    
    /* Initializing the SysTick Timer */
    InitSystickTimer(SysTick_Callback);
    StartSystickTimer(HEARTBEAT_PERIOD);
    
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
    if (isHeartbeat) {
        isHeartbeat = false;
        StopSystickTimer();
        rtcCounter = RTC_GetCounter();
        /* Send RTC value */
        USB_SendToHost(eGetRtcCmd, sizeof(rtcCounter), (uint8_t*)&rtcCounter);
        StartSystickTimer(HEARTBEAT_PERIOD);
        /* Reload watchdog */
        #ifndef DEBUG
        WatchdogReload(KR_KEY_Reload);
        #endif
    }
}

/*!
 \brief RTC second event callback function
*/
void RTC_SecondEvent(void)
{
    /* Start periodic polling of the 1-Wire bus */
    if (isOnewireReadEnabled) {
        // Schedule a task to read from 1-wire devices
        PutTask(DeviceReadTask, &devType);
    }
}

/*!
 \brief SysTick callback function
*/
void SysTick_Callback(void)
{
    isHeartbeat = true;
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
        	case eSearchCmd:
                // Schedule a task to search a 1-wire devices
                PutTask(DeviceSearchTask, NULL);
        		break;
            case eReadCmd:
                devType = rx_packet->data[0];
                isOnewireReadEnabled = true;
        		break;
            case eWriteCmd:
                // Schedule a task to write at 1-wire devices
                PutTask(DeviceWriteTask, rx_packet->data);
        		break;
            case eSyncRtcCmd:
                disableRtcInterrupt(RTC_IT_SEC);
                rtcCounter = *((uint32_t*)rx_packet->data);
                presetDateTime(rtcCounter);
                enableRtcInterrupt(RTC_IT_SEC);
                break;
        	default:
        		break;
        }
    }
}
