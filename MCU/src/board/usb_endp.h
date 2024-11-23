#ifndef __USB_ENDP_H
#define __USB_ENDP_H

#include "usb_lib.h"

typedef struct
{
    uint8_t *rx_buffer;
    void(*rx_handler)(void);
} HidEndp_t;

/*  */
void USB_SetHidEndp(HidEndp_t *endp);
#endif // __USB_ENDP_H
