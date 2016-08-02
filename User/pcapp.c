#include "pcapp.h"
#include "usb2com.h"
#include "spinrf.h"

#define USB_RX_BUFFTER_SIZE 32
#define USB_TX_BUFFTER_SIZE 32

uint8_t USB_RX_BUFFTER[USB_RX_BUFFTER_SIZE];

uint8_t radioTxFlag;
volatile uint16_t radioTxLoopCount;

void TIM1_UP_IRQHandler(void)
{
	if(radioTxLoopCount ++ == 100)
	{
		radioTxFlag = 1;
		radioTxLoopCount = 0;
	}
	TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);  
}

void applicationLoop(void)
{
	static uint8_t i;
	if(radioTxFlag == 1)
	{
		radioTxFlag = 0;
		if(pcDataReceive() == 1)
		{
			//nrfIqrTx_Packet(NRF24L01_TXDATA);
		}
	}
}

void applicationLoopInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  //打开时钟
    
    TIM_DeInit(TIM1);

    TIM_TimeBaseStructure.TIM_Period = 720;//定时1/10ms
    TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//预分频 72/(10-1 +1)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
    /* NVIC_PriorityGroup 2 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//飞控主循环基准定时器，优先级高于串口打印
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM1,ENABLE);

}

uint8_t pcDataReceive(void)
{
	uint8_t length,i;
	length = USB_RxRead(USB_RX_BUFFTER, sizeof(USB_RX_BUFFTER));
	if(length >0)
	{
		for(i=0;i<32;i++)
		//NRF24L01_TXDATA[i] = USB_RX_BUFFTER[i];
		return 1;
	}
}

