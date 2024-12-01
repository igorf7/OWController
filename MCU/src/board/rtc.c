/*!
 \file   rtc.c
 \date   November-2024 (created)
 \brief  RTC driver
 */
#include "rtc.h"

static RtcEvents_t *rtcEvents;
                        
unsigned char rtc_init(RtcEvents_t *events)
{
    rtcEvents = events; // init callbacks
    
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    /* Если RTC выключен - инициализировать */
    if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN)
    {            
        /* Reset Backup Domain */
        BKP_DeInit();
        RCC_LSEConfig(RCC_LSE_ON);      // LSE with qurtz
        while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY){}
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        /* Set RTC prescaler: set RTC period to 1sec */
        RTC_SetPrescaler(0x7FFF); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        return 1;
    }
    return 0;
}

void rtc_set_irq(uint16_t irq_event)
{
    /* Configure irq priority */
    //NVIC_SetPriority(RTC_IRQn, NVIC_EncodePriority(1, 0, 0));
    NVIC_SetPriority(RTC_IRQn, 0);
    /* Enable the RTC Interrupt */
    NVIC_EnableIRQ(RTC_IRQn);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Enable the RTC IRQ */
    RTC_ITConfig(irq_event, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

void disableRtcInterrupt(uint16_t irq_event)
{ 
    RTC_ITConfig(irq_event, DISABLE);
}

void enableRtcInterrupt(uint16_t irq_event)
{
    RTC_WaitForLastTask();
    RTC_ITConfig(irq_event, ENABLE);
    RTC_WaitForLastTask();
}

uint32_t presetDateTime(uint32_t datetime)
{
    RTC_SetCounter(datetime);
    
    return datetime;
}

//void rtcClkOutput(void)
//{
//    /* Вывод RTC_Clock/64 на PC13 */
//    // Disable the Tamper Pin
//    BKP_TamperPinCmd(DISABLE); // To output RTCCLK/64 on Tamper pin, the tamper
//                               // functionality must be disabled
//    // Enable RTC Clock Output on Tamper Pin
//    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
//}

/* RTC second interrupt */
void RTC_IRQHandler(void)
{    
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);
        
        RTC_WaitForSynchro();
        
        if( (rtcEvents != NULL) && (rtcEvents->secondEvent != NULL) ){
            rtcEvents->secondEvent(); // notifies application about RTC event
        }
    }
}

