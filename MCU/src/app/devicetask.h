/*!
 \file   devicetask.h
 \date   November-2024 (created)
 \brief  device task handlers header file
*/
#ifndef __DEVIETASK_H
#define __DEVIETASK_H

#include "stm32f10x.h"
#include "usb_pwr.h"
#include "usb_desc.h"
#include "usb_endp.h"
#include "usb_prop.h"
#include "usb_hw_config.h"
#include "usb_lib.h"

/* API */
void TaskDeviceSearch(void *prm);
void TaskDeviceEnumerate(void *prm);
void TaskDeviceRead(void *prm);
void TaskDeviceWrite(void *prm);
void TaskSetOwDataRequest(bool state);
uint8_t DeviceGetCount(void);
#endif // __DEVIETASK_H
