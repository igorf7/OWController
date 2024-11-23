#ifndef RTC_H
#define RTC_H

#include "stm32f10x.h"

/* RTC events callback functions */
typedef struct{
    void(*secondEvent)(void);
}RtcEvents_t;

unsigned char rtc_init(RtcEvents_t *events);
void rtc_set_irq(uint16_t irq_event);
void disableRtcInterrupt(uint16_t irq_event);
void enableRtcInterrupt(uint16_t irq_event);
char* getWdayStr(uint8_t daycode);
void rtcClkOutput(void);
uint32_t presetDateTime(uint32_t datetime);
#endif
//eof
