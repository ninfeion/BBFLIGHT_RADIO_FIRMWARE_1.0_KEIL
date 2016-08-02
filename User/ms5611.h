#ifndef _MS5611_H_
#define _MS5611_H_
#include "stm32f10x.h"

#define MS5611_ADDRESS		       0XEE //111011CX Cȡ����CSB�Ĳ���

#define CMD_MS5611_RESET  		   0x1E
#define CMD_MS5611_ADC_RD    		 0x00
#define	CMD_MS5611_PROM_RD 	 		 0xA0
#define CMD_MS5611_PROM_CRC  		 0xAE

#define	CMD_MS5611_D1_OSR_256    0x40
#define	CMD_MS5611_D1_OSR_512    0x42
#define	CMD_MS5611_D1_OSR_1024   0x44
#define	CMD_MS5611_D1_OSR_2048   0x46
#define	CMD_MS5611_D1_OSR_4096   0x48

#define	CMD_MS5611_D2_OSR_256    0x50
#define	CMD_MS5611_D2_OSR_512    0x52
#define	CMD_MS5611_D2_OSR_1024   0x54
#define	CMD_MS5611_D2_OSR_2048   0x56
#define	CMD_MS5611_D2_OSR_4096   0x58

//PROM constants
//A0 	  16 bit reserved for manufacturer
//A2 C1 PRESSURE SENSITIVITY
//A4 C2 PRESSURE OFFSET
//A6 C3 TEMPERATURE COEFFICIENT OF PRESSURE SENSITIVITY
//A8 C4 TEMPERATURE COEFFICIENT OF PRESSURE OFFSET
//AA C5 REFERENCE REMPERATURE
//AC C6 TEMPERATURE COEFFICIENT OF THE TEMPERATURE

// OSR (Over Sampling Ratio) constants
//MS5611_OSR_256 		Conversion time 0.6ms  Resolution 0.065mbar
//MS5611_OSR_512 		Conversion time 1.2ms  Resolution 0.042mbar
//MS5611_OSR_1024 	Conversion time 2.3ms  Resolution 0.027mbar
//MS5611_OSR_2048 	Conversion time 4.6ms  Resolution 0.018mbar
//MS5611_OSR_4096 	Conversion time 9.1ms  Resolution 0.012mbar

#define ConversionTime 10000
#define DefaultPresInitFilterTime 50

//D1����ѹ��D2���¶�

void MS5611_Init(void);
void MS5611_Reset(void);
void MS5611_PROM_READ(uint16_t *PROM_C);
uint32_t MS5611_SendCmd_StartConversionAndGetIt(uint8_t OSR_Cmd_SelectTempOrPres);
void MS5611_GetTempture(uint8_t SelectOsr, uint16_t *PROM_C, float *TEMPERATURE_DATA);
void MS5611_GetPressure(uint8_t SelectOsr, uint16_t *PROM_C, float *TEMPERATURE_DATA, float *AfterOffset_TempPresDATA);
uint8_t MS5611_GetAltitude(float *AfterOffset_TempPresDATA, float *Altitude_DATA);//return 0 �������������ȷ�߶�

#endif


