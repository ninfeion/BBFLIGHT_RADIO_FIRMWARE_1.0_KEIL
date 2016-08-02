#include "adc.h"
#include "stm32f10x_dma.h"


//TCAD->PA0->ADC12_IN0
//AD_VBAT->PA1->ADC12_IN1

#define BATTERYCHANNEL 1
//#define CURRENTCHANNEL 0
//#define CHANNELNUM `2 //使用的通道数
#define ADC_MEAN_SIZE	 10
#define BATTERY_CHANNEL ADC_Channel_1
//#define CURRENT_CHANNEL ADC_Channel_0

//volatile uint16_t adcDMAValues[ADC_MEAN_SIZE];

void adcInit(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚 
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
		
	BATTERY_0.BatReal = 3950;//单位为mv 电池实际电压  校准电压时修改
  BATTERY_0.ADinput = 1980;//单位为mv R15和R17连接处电压 校准电压时修改
  BATTERY_0.ADRef   = 3000;//单位为mv 单片机供电电压   校准电压时修改
  BATTERY_0.Bat_K   = 2;//计算电压计算系数
  BATTERY_0.BatteryADmin = 2000;//电压门限AD值
	
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
	BATTERY_0.BatteryVal = BATTERY_0.Bat_K * BATTERY_0.BatteryAD * BATTERY_0.ADRef / 4096;//实际电压 值计算	
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
	ADC_InitTypeDef ADC_InitStructure;//定义ADC初始化结构体变量
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE ); 
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                       //72M/6=12,ADC最大时间不能超过14M
        
  ADC_DeInit(ADC1); 
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//有软件控制转换
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 2;//转换通道数目为2
  ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC
	
  //设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期  
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 ); 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );//TCONV = 采样周期+ 12.5个周期 
	
  ADC_DMACmd(ADC1, ENABLE);//使能ADC1模块DMA  // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
	//ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器功能:
  ADC_Cmd(ADC1, ENABLE);//使能ADC1
	
  ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
  while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
  ADC_StartCalibration(ADC1);//开始ADC1校准
  while(ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
  
	ADC_DMA_Configuration();
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能ADC1软件开始转换

	BATTERY_0.BatReal = 3950;//单位为mv 电池实际电压  校准电压时修改
  BATTERY_0.ADinput = 1980;//单位为mv R15和R17连接处电压 校准电压时修改
  BATTERY_0.ADRef   = 3000;//单位为mv 单片机供电电压   校准电压时修改
  BATTERY_0.Bat_K   = 2;//计算电压计算系数
  BATTERY_0.BatteryADmin = 2000;//电压门限AD值
	
	CURRENT_0.ADRef = 3000;
	CURRENT_0.gain = 100.0;
	CURRENT_0.Rom = 10;
	return 1;
}

void ADC_DMA_Configuration(void) 
{   
	DMA_InitTypeDef DMA_InitStructure;  
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; //DMA外设ADC基地址 
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_DMAValue; //DMA内存基地址  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地 
	DMA_InitStructure.DMA_BufferSize = ChannelNum * BAT_filter_times;//DMA通道的DMA缓存的大小
																													//两个通道 一个采集八次
  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位  
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位  
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输  
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道  
	
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
	
	CURRENT_0.CurrentAD =  ADCTEMP / ADC_MEAN_SIZE;  //右移3位=除以8
	CURRENT_0.CURRENTVal = CURRENT_0.CurrentAD * CURRENT_0.ADRef / 4096 / CURRENT_0.Rom / CURRENT_0.gain ;
	
	return CURRENT_0.CURRENTVal;
}
*/
