#include "adc.h"
#include "stm32f10x_dma.h"


//TCAD->PA0->ADC12_IN0
//AD_VBAT->PA1->ADC12_IN1

#define BATTERYCHANNEL 1
//#define CURRENTCHANNEL 0
//#define CHANNELNUM `2 //ʹ�õ�ͨ����
#define ADC_MEAN_SIZE	 10
#define BATTERY_CHANNEL ADC_Channel_1
//#define CURRENT_CHANNEL ADC_Channel_0

//volatile uint16_t adcDMAValues[ADC_MEAN_SIZE];

void adcInit(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ���������� 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA,ENABLE);
		
	// ADC1 configuration
  ADC_DeInit(ADC1);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  // Enable ADC1
  ADC_Cmd(ADC1, ENABLE);
  // Calibrate ADC1
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
		
	BATTERY_0.BatReal = 3950;//��λΪmv ���ʵ�ʵ�ѹ  У׼��ѹʱ�޸�
  BATTERY_0.ADinput = 1980;//��λΪmv R15��R17���Ӵ���ѹ У׼��ѹʱ�޸�
  BATTERY_0.ADRef   = 3000;//��λΪmv ��Ƭ�������ѹ   У׼��ѹʱ�޸�
  BATTERY_0.Bat_K   = 2;//�����ѹ����ϵ��
  BATTERY_0.BatteryADmin = 2000;//��ѹ����ADֵ
	
	CURRENT_0.ADRef = 3000;
	CURRENT_0.gain = 100.0;
	CURRENT_0.Rom = 10;
}

uint16_t adcBatteryConversion(void)
{
	uint16_t ADCTEMP;
	uint8_t i;
	for(i=0;i< ADC_MEAN_SIZE;i++)
	{
		ADCTEMP += adcGetValues(BATTERY_CHANNEL);
	}
	BATTERY_0.BatteryAD = ADCTEMP / ADC_MEAN_SIZE ;  
	BATTERY_0.BatteryVal = BATTERY_0.Bat_K * BATTERY_0.BatteryAD * BATTERY_0.ADRef / 4096;//ʵ�ʵ�ѹ ֵ����	
	return BATTERY_0.BatteryVal;
}

uint16_t adcGetValues(uint8_t channel)
{
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	
	return ADC_GetConversionValue(ADC1);
}


/*
uint8_t ADCInit(void)
{
	ADC_InitTypeDef ADC_InitStructure;//����ADC��ʼ���ṹ�����
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ���������� 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE ); 
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                       //72M/6=12,ADC���ʱ�䲻�ܳ���14M
        
  ADC_DeInit(ADC1); 
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1��ADC2�����ڶ���ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ʹ��ɨ��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADCת������������ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//���������ת��
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ת�������Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 2;//ת��ͨ����ĿΪ2
  ADC_Init(ADC1, &ADC_InitStructure); //��ʼ��ADC
	
  //����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	//ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����  
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 ); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );//TCONV = ��������+ 12.5������ 
	
  ADC_DMACmd(ADC1, ENABLE);//ʹ��ADC1ģ��DMA  // ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
	//ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ���������:
  ADC_Cmd(ADC1, ENABLE);//ʹ��ADC1
	
  ADC_ResetCalibration(ADC1); //����ADC1У׼�Ĵ���
  while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�ADC1У׼�������
  ADC_StartCalibration(ADC1);//��ʼADC1У׼
  while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADC1У׼���
  
	ADC_DMA_Configuration();
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ADC1�����ʼת��

	BATTERY_0.BatReal = 3950;//��λΪmv ���ʵ�ʵ�ѹ  У׼��ѹʱ�޸�
  BATTERY_0.ADinput = 1980;//��λΪmv R15��R17���Ӵ���ѹ У׼��ѹʱ�޸�
  BATTERY_0.ADRef   = 3000;//��λΪmv ��Ƭ�������ѹ   У׼��ѹʱ�޸�
  BATTERY_0.Bat_K   = 2;//�����ѹ����ϵ��
  BATTERY_0.BatteryADmin = 2000;//��ѹ����ADֵ
	
	CURRENT_0.ADRef = 3000;
	CURRENT_0.gain = 100.0;
	CURRENT_0.Rom = 10;
	return 1;
}

void ADC_DMA_Configuration(void) 
{   
	DMA_InitTypeDef DMA_InitStructure;  
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel1); //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; //DMA����ADC����ַ 
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_DMAValue; //DMA�ڴ����ַ  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //�ڴ���Ϊ���ݴ����Ŀ�ĵ� 
	DMA_InitStructure.DMA_BufferSize = ChannelNum * BAT_filter_times;//DMAͨ����DMA����Ĵ�С
																													//����ͨ�� һ���ɼ��˴�
  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ������� 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ������� 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���ݿ��Ϊ16λ  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //������ѭ������ģʽ  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ�  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��  
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��  
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
} 

uint16_t ADC_CONV_CURRENT(void)
{
	uint16_t ADCTEMP;
	uint8_t i;
		
	for(i=0;i<ADC_MEAN_SIZE;i++)
	{
		ADCTEMP += adcDMAValues[i][CURRENTCHANNEL];
	}
	
	CURRENT_0.CurrentAD =  ADCTEMP / ADC_MEAN_SIZE;  //����3λ=����8
	CURRENT_0.CURRENTVal = CURRENT_0.CurrentAD * CURRENT_0.ADRef / 4096 / CURRENT_0.Rom / CURRENT_0.gain ;
	
	return CURRENT_0.CURRENTVal;
}
*/
