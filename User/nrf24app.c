#include "nrf24app.h"

/* EXT_5 -> PB2 -> 3 -> MOSI(MCU)
   EXT_4 -> PB10 -> 4 -> SCK
   EXT_7 -> PA15 -> 2 -> CSN
   EXT_6 -> PB3 -> 1 -> CE
   D -> PA3 -> IRQ
   U -> PA2 -> MISO(MCU)
   */


void virtualSpiIoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ; // CSN 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  // IRQ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10; // CE SCK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2; // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;      
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
}

uint8_t SPI_SendByte(uint8_t dt)  
{  
    uint8_t i;
    uint8_t temp=0;  
    for(i=8;i>0;i--)  
    {  
        if(dt&0x80)
		{
			MOSI_H;  
		}
        else 
		{
			MOSI_L;  
		}
        dt<<=1;  
		
        SCLK_H;  
        __nop();  
        __nop();  
        __nop(); 


        temp<<=1;  
        if(MISO)
		{
			temp++;  
		}
		
       
        __nop();  
        __nop();  
        __nop(); 
		 SCLK_L;
    }  
    return temp;  
}  

//Ð´»º³åÇø
uint8_t VNRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
    uint8_t i;
    uint8_t status;
    CSN_L;				        
    status = SPI_SendByte(reg);	            
    for(i=0; i<uchars; i++)
    {
        SPI_SendByte(pBuf[i]);		        
    }
    CSN_H;						
    return 	status;	
}


//¶Á»º³åÇø
uint8_t VNRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	//CE_L();
	CSN_L;					    
	status = SPI_SendByte(reg);	           
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = SPI_SendByte(0);            	
	}
	CSN_H;					   
	return status;
}

//Ð´¼Ä´æÆ÷
uint8_t VNRF_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    CSN_L;					  
    status = SPI_SendByte(reg);  
    SPI_SendByte(value);        
    CSN_H;          
    return 	status;
}


//¶Á¼Ä´æÆ÷
uint8_t VNRF_Read_Reg(uint8_t reg)
{
    uint8_t reg_val;
    CSN_L;					 
    SPI_SendByte(reg);			  
    reg_val = SPI_SendByte(0);   
    CSN_H;	
 
    return 	reg_val;
}

uint8_t VNRF24L01_Check(void) 
{ 
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2}; 
	u8 buf1[5]; 
	u8 i; 
    
	VNRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5); 
	VNRF_Read_Buf(TX_ADDR,buf1,5); 
   
	for (i=0;i<5;i++) 
	{ 
		if (buf1[i]!=0xC2) 
		break; 
	} 
	if (i==5)  
		return 1;
	else         
		return 0;   
} 


   