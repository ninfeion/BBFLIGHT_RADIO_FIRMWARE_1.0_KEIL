#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "stm32f10x.h"

#define USB_PRINTF_BUFFER_SIZE 128

void DEBUG_TIM2_Init(void);
void USB_printf(uint8_t *TarStr, ...);
void USB_DEBUG_RADIO(void);

extern uint8_t USB_DEBUG_flag; 
extern volatile uint16_t USB_DEBUG_flag_count;
#endif


