/*!
 \file   rtc.h
 \date   November-2024 (created)
 \brief  RTC driver header file
 */
#ifndef RTC_H
#define RTC_H

#include "stm32f10x.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

/* RTC events callback functions */
typedef struct{
    void(*secondEvent)(void);
}RtcEvents_t;

/* API */
unsigned char rtc_init(RtcEvents_t *events);
void rtc_set_irq(uint16_t irq_event);
void disableRtcInterrupt(uint16_t irq_event);
void enableRtcInterrupt(uint16_t irq_event);
//void rtcClkOutput(void);
uint32_t presetDateTime(uint32_t datetime);
#endif
