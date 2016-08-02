#include "pwm.h"

//NRF_CLK-> PA9  T1C2
//MOTO_0->PB9 T4C4
//MOTO_2->PB0	T3C3
//MOTO_1->PB8 T4C3
//MOTO_3->PB1	T3C4

 
uint8_t PWMInit(void)
{
	
//	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO��ʼ���ṹ��
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
//  TIM_OCInitTypeDef  TIM_OCInitStructure;  
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 
	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 

  //PWM��·��GPIO��ʼ��
  //MOTO_0->PB9
  //MOTO_2->PB0
  //MOTO_1->PB8
  //MOTO_3->PB1
	
	MOTO_Init(&MOTO_A);
	
	MOTO_Init(&MOTO_B);
	
	MOTO_Init(&MOTO_C);
	
	MOTO_Init(&MOTO_D);
	
	return 1;
}

uint8_t MOTO_Init(const MOTO_DEF *MOTO_MAP)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
	
	RCC_APB2PeriphClockCmd( MOTO_MAP->Gpiorcc_Periph | RCC_APB2Periph_AFIO, ENABLE); 
	
	if(MOTO_MAP->TIMX ==TIM3 || MOTO_MAP->TIMX	== TIM4)
	RCC_APB1PeriphClockCmd( MOTO_MAP->Gpiorcc_Tim_Periph, ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = MOTO_MAP->Gpio_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_2MHz;
	GPIO_Init(MOTO_MAP->Gpiox, &GPIO_InitStructure);
	
	//TIM_DeInit(MOTO_MAP->TIMX);
	
	// PWM timer configuration 
	TIM_TimeBaseStructure.TIM_Period = MOTO_MAP->TimPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = MOTO_MAP->TimPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //0+1 = ������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  //�ظ������������ظ�������ٴβŸ�����һ������ж�
	TIM_TimeBaseInit(MOTO_MAP->TIMX, &TIM_TimeBaseStructure);
	
	
	// PWM channels configuration (All identical!)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = MOTO_MAP->TimPulse;
    TIM_OCInitStructure.TIM_OCPolarity = MOTO_MAP->TimPolarity;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	MOTO_MAP->ocInit(MOTO_MAP->TIMX, &TIM_OCInitStructure);
	
	TIM_CtrlPWMOutputs(MOTO_MAP->TIMX, ENABLE);
	
	TIM_Cmd(MOTO_MAP->TIMX,ENABLE);

	return 1;
}

//pwm���º���
void MotorPwmFlash(int32_t MOTO1_PWM,int32_t MOTO2_PWM,int32_t MOTO3_PWM,int32_t MOTO4_PWM)
{		
     if(MOTO1_PWM>=Moto_PwmMax)	MOTO1_PWM = Moto_PwmMax;
     if(MOTO2_PWM>=Moto_PwmMax)	MOTO2_PWM = Moto_PwmMax;
     if(MOTO3_PWM>=Moto_PwmMax)	MOTO3_PWM = Moto_PwmMax;
     if(MOTO4_PWM>=Moto_PwmMax)	MOTO4_PWM = Moto_PwmMax;
     if(MOTO1_PWM<=0)	MOTO1_PWM = 0;
     if(MOTO2_PWM<=0)	MOTO2_PWM = 0;
     if(MOTO3_PWM<=0)	MOTO3_PWM = 0;
     if(MOTO4_PWM<=0)	MOTO4_PWM = 0;//�޶����벻��С��0������999
    
	 MOTO_A.TIMX->CCR4 = MOTO1_PWM;
     MOTO_B.TIMX->CCR3 = MOTO1_PWM;
     MOTO_C.TIMX->CCR3 = MOTO2_PWM;
     MOTO_D.TIMX->CCR4 = MOTO3_PWM;        //�Զ�ʱ���Ĵ�����ֵ
}
