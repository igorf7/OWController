#ifndef __DEVIETASK_H
#define __DEVIETASK_H

#include "stm32f10x.h"
#include "usb_pwr.h"
#include "usb_desc.h"
#include "usb_endp.h"
#include "usb_prop.h"
#include "usb_hw_config.h"
#include "usb_lib.h"

/*  */
void DeviceSearchTask(void *prm);
void DeviceEnumerate(void *prm);
void DeviceReadTask(void *prm);
#endif // __DEVIETASK_H
