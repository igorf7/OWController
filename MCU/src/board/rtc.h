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
void RTC_Init(RtcEvents_t *events);
void RTC_Set_Irq(uint16_t irq_event);
void RTC_DisableInterrupt(uint16_t irq_event);
void RTC_EnableInterrupt(uint16_t irq_event);
//void RTC_ClkOutput(void);
uint32_t RTC_PresetDateTime(uint32_t datetime);
#endif
