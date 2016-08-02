#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f10x.h"

typedef struct 
{
	uint16_t  BatteryAD;             //��ѹADֵ
	uint16_t  BatteryVal;            //��ѹʵ��ֵ
	uint16_t  BatReal;               //��ص�ʵ�ʵ�ѹ�������ñ��
	uint16_t  ADRef;                 //AD�ο�Դ��ѹ�������ǵ�Ƭ�������ѹ��һ����3.3V���ң�Ҫʵ��
	uint16_t  ADinput;               //AD���������ѹ--->R15��R17�����ĺ��̵�ѹ
	uint16_t  Bat_K;                 //�����ѹֵϵ�������ڵ�ѹУ׼
	uint16_t  BatteryADmin;          //��ѹ����
	char   alarm;									//����λ
	char   chargeSta;							//���״̬
}BATTERY_TypeDef;

typedef struct
{
	uint16_t CurrentAD;
	float gain;                   //�˷�����
	uint16_t ADRef;                  //�ο���ѹ mv
	uint16_t Rom;										//���������С
	uint16_t CURRENTVal;
	
}CURRENT_TypeDef;


static BATTERY_TypeDef BATTERY_0;
static CURRENT_TypeDef CURRENT_0;

void adcInit(void);
uint16_t adcBatteryConversion(void);
uint16_t adcGetValues(uint8_t channel);
//void ADC_DMA_Configuration(void);
//uint16_t ADC_CONV_CURRENT(void);

#endif
