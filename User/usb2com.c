#include"usb2com.h"

void usbVirtualComInit(void)
{
	USB_Config();
}

void USB_printf(uint8_t *TarStr, ...)
{
	__va_list args;
		
	uint8_t BUF[USB_PRINTF_BUFFER_SIZE];
	uint8_t length;
		
	va_start(args, TarStr);
	length = vsprintf(BUF, TarStr, args);
	USB_TxWrite(BUF,length);
	va_end(args);
}


