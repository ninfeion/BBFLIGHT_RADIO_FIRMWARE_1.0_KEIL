#ifndef _USB2COM_H_
#define _USB2COM_H_
#include "stm32f10x.h"
#include "hw_config.h"

#include "stdio.h"
#include "stdarg.h"

#define USB_PRINTF_BUFFER_SIZE 128

void usbVirtualComInit(void);
void USB_printf(uint8_t *TarStr, ...);

#endif

