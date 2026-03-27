/**
 * timer.h
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include "stm32f10x.h"

void Tim2Init(void);
void WaitMicrosec(uint16_t uSec);
void WaitMillisec(uint16_t mSec);
#endif
