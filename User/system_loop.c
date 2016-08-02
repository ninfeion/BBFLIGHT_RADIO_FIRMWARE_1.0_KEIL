#include "system_loop.h"
#include "spinrf.h"
#include "pcapp.h"

#include"system_config.h"
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

void systemLoop(void)
{
	static uint8_t i;
	if(radioTxFlag == 1)
	{
		radioTxFlag = 0;
		if(pcDataReceive() == 1)
		{
			nrfIqrTx_Packet(NRF24L01_TXDATA);
		}
	}
}

void SYSTEM_LOOP_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  //��ʱ��
    
    TIM_DeInit(TIM1);

    TIM_TimeBaseStructure.TIM_Period = 720;//��ʱ1/10ms
    TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//Ԥ��Ƶ 72/(10-1 +1)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
    /* NVIC_PriorityGroup 2 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//�ɿ���ѭ����׼��ʱ�������ȼ����ڴ��ڴ�ӡ
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM1,ENABLE);

}
