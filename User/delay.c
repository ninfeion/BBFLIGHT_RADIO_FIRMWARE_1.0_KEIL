#include"delay.h"



__IO uint32_t TimingDelay;

/*延时1us函数*/
void delay_us(__IO uint32_t usnTime)
{
  TimingDelay = usnTime;
  while(TimingDelay != 0);
}

extern __IO uint32_t TimingDelay;

void delay_ms(__IO uint32_t msnTime)
{
  TimingDelay = msnTime * 1000;
  while(TimingDelay != 0);
}


/*stm32f10x_it.c中的void SysTick_Handler(void)函数改为*/
void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

void delay_noInt_ms(uint32_t time)
{
	time *= 1429;//50000 -> 35.006ms
	while(time --);
}
