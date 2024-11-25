#include "main.h"
#include "scheduler.h"
#include "devicetask.h"

static HidProp_t hidProp;
static HidEndp_t hidEndp;
static RtcEvents_t rtcEvents;
static uint32_t rtcCounter;
static bool isOnewireReadEnabled = false;
static uint8_t devType = DS1971;
static uint8_t usbRxBuffer[wMaxPacketSize];

/* Program entry point */
int main(void)
{
    // Initialize the USB hardware
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
    
    // Program startup indicator
    LED_Init(LED1_PORT, LED1_PIN);
    LED_Blink(LED1_PORT, LED1_PIN, 1000000);
    
    // Initialize the task scheduler
    InitTaskSheduler(&BackgroundTask);
        
    // Initialize the 1-Wire Bus
    OW_InitBus(USART2);
    
    // Initialize the RTC hardware
    rtcEvents.secondEvent = onSecondEvent;
    rtc_init(&rtcEvents);
    rtc_set_irq(RTC_IT_SEC);
    
    // Search 1-Wire devices
    //DeviceSearchTask(NULL);
    
//    // Enable Watchdog
//    #ifndef DEBUG
//    InitWatchdog();
//    #endif
    
    //isOnewireReadEnabled = true;

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
//    #ifndef DEBUG
//    WatchdogReload(KR_KEY_Reload);
//    #endif
}

/* On RTC event callback function */
/*!
 \brief RTC second event callback function
*/
void onSecondEvent(void)
{
    rtcCounter = RTC_GetCounter();
    
    // Send RTC value
    USB_SendToHost(eGetRtcCmd, sizeof(rtcCounter), (uint8_t*)&rtcCounter);
    
    // Schedule a task to read a 1-wire devices
    if (isOnewireReadEnabled) {
        PutTask(DeviceReadTask, &devType);
    }
}

/*!
 \brief
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
