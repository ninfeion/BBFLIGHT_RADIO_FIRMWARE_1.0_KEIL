#include "debug.h"
#include "usb2com.h"
#include "adc.h"
#include "system_config.h"
#include "systeminit.h"
#include "delay.h"

#include "stdio.h"
#include "stdarg.h"
//#include "string.h"

//struct SYSTEM_STATE BBSYSTEM;

uint8_t USB_DEBUG_flag; 
volatile uint16_t USB_DEBUG_flag_count;

/*
void TIM2_IRQHandler(void)
{		
	USB_DEBUG_flag_count ++;
	if(USB_DEBUG_flag_count == 1000) //1hz
	{
		USB_DEBUG_flag=1;
		USB_DEBUG_flag_count = 0;
	}
	
	TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
}*/

void TIM2_IRQHandler(void)
{
	
}

void DEBUG_TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //打开时钟
    
	TIM_DeInit(TIM2);

	TIM_TimeBaseStructure.TIM_Period = 7200-1;//定时1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//预分频 72/(10-1 +1)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
	 
	/* NVIC_PriorityGroup 2 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//串口打印定时器，优先级低于姿态解算
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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
	
void USB_DEBUG_RADIO(void)
{
	uint16_t ADCTEMP;
	
	ADCTEMP = adcBatteryConversion();

  if(BBSYSTEM.NRFONLINE==1)
		USB_printf("NRF is online!\n");
	
  USB_printf("The BatVolt is: %d mv\n",ADCTEMP);
	USB_printf("=================================\n");
}


