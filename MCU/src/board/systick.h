/*!
 \file   systick.h
 \date   October-2024 (created)
 \brief  SysTick driver interface
 \author alcont application team
*/
#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f10x.h"
#include "stddef.h"

void InitSystickTimer(void (*callback)(void));
void StartSystickTimer(uint32_t ms);
void StopSystickTimer(void);
#endif // SYSTICK_H
