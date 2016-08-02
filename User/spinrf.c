#include "spinrf.h"
#include "systeminit.h"
#include "delay.h"

//�޸ĸý��պͷ��͵�ַ�����Թ������������ͬһ������У����ݲ��ܸ���
static uint8_t  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0xc3,0x10,0x10,0x00};	//������ַ	
static uint8_t  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0xc3,0x10,0x10,0x11};	//���յ�ַ

uint8_t spi1NrfInit(void)
{
	SPI_InitTypeDef SPI_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO |RCC_APB2Periph_SPI1, ENABLE );        //spi2�˿�ʱ��ʹ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  // IRQ
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
		
	SPI_CSN_H;
		
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫�� 
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //��ģʽ 
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //���ݴ�С8λ 
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //ʱ�Ӽ��ԣ�����ʱΪ�� 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //��1��������Ч��������Ϊ����ʱ�� 
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS�ź����������� 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //8��Ƶ��9MHz 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //��λ��ǰ 
    SPI_InitStructure.SPI_CRCPolynomial = 7; 
    SPI_Init(SPI_X, &SPI_InitStructure); 
	SPI_Cmd(SPI_X, ENABLE);
		
	return 1;
}


uint8_t NRF_TxPacket(uint8_t *tx_buf)
{
	uint8_t state;  
	SPI_CE_L;

	/*д���ݵ�TX BUF ��� 32���ֽ�*/						
    NRF_Write_Buf(WR_TX_PLOAD,tx_buf,TX_PLOAD_WIDTH);

     /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */   
 	SPI_CE_H;
	  	
	/*�ȴ���������ж� */                            
	while(NRF_Read_IRQ()!=0); 	
	
	/*��ȡ״̬�Ĵ�����ֵ */                              
	state = NRF_Read_Reg(NRF_STATUS);
	
	 /*���TX_DS��MAX_RT�жϱ�־*/                  
	NRF_Write_Reg(NRF_WRITE_REG+NRF_STATUS,state); 	

	NRF_Write_Reg(FLUSH_TX,NOP);    //���TX FIFO�Ĵ��� 

	 /*�ж��ж�����*/    
	if(state&MAX_RT)                     //�ﵽ����ط�����
			 return MAX_RT; 

	else if(state&TX_DS)                  //�������
		 	return TX_DS;
	else						  
			return ERROR;                 //����ԭ����ʧ��
}
  
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;		
	SPI_CE_H;
	while(NRF_Read_IRQ()!=0); 
	SPI_CE_L;
	state = NRF_Read_Reg(NRF_STATUS); 
	//��ȡ״̬�Ĵ�����ֵ    	 
	NRF_Write_Reg(NRF_WRITE_REG + NRF_STATUS,state); //����жϱ�־
	if(state&RX_DR)//���յ�����
	{
		NRF_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF_Write_Reg(FLUSH_RX,NOP);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   		
	else
	return 1;//û�յ��κ�����
}	


uint8_t NRF24L01_Check(void) 
{ 
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2}; 
	u8 buf1[5]; 
	u8 i; 
    
	/*д��5 ���ֽڵĵ�ַ.*/ 
	NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5); 
     
	/*����д��ĵ�ַ */ 
	NRF_Read_Buf(TX_ADDR,buf1,5); 
   
	/*�Ƚ�*/ 
	for (i=0;i<5;i++) 
	{ 
		if (buf1[i]!=0xC2) 
		break; 
	} 
	if (i==5)  
		return 1;
	else         
		return 0;  //MCU��NRF����������    
} 

uint8_t SPI_RW(uint8_t dat) 
{ 
    while (SPI_I2S_GetFlagStatus(SPI_X, SPI_I2S_FLAG_TXE) == RESET); 
    SPI_I2S_SendData(SPI_X, dat); 
    while (SPI_I2S_GetFlagStatus(SPI_X, SPI_I2S_FLAG_RXNE) == RESET);  
    return SPI_I2S_ReceiveData(SPI_X); 
}

//д������
uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
    uint8_t i;
    uint8_t status;
    SPI_CSN_L;				        /* ѡͨ���� */
    status = SPI_RW(reg);	            /* д�Ĵ�����ַ */
    for(i=0; i<uchars; i++)
    {
        SPI_RW(pBuf[i]);		        /* д���� */
    }
    SPI_CSN_H;						/* ��ֹ������ */
    return 	status;	
}


//��������
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	//SPI_CE_L;
	SPI_CSN_L;					    /* ѡͨ���� */
	status = SPI_RW(reg);	            /* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = SPI_RW(0);            /* ��ȡ�������� */ 	
	}
	SPI_CSN_H;					    /* ��ֹ������ */
	return status;
}

//д�Ĵ���
uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    SPI_CSN_L;					  
    status = SPI_RW(reg);  
    SPI_RW(value);         /* д���� */
    SPI_CSN_H;           /* ��ֹ������ */
    return 	status;
}


//���Ĵ���
uint8_t NRF_Read_Reg(uint8_t reg)
{
    uint8_t reg_val;
    SPI_CSN_L;					 
    SPI_RW(reg);			  
    reg_val = SPI_RW(0);   /* ��ȡ�üĴ����������� */
    SPI_CSN_H;	
 
    return 	reg_val;
}

void nrf_RXMode(void)
{	
	SPI_CE_L;
	NRF_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);
	NRF_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);
	NRF_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x01);
	NRF_Write_Reg(NRF_WRITE_REG + SETUP_AW, 0x05);
	NRF_Write_Reg(NRF_WRITE_REG + RF_CH, 0x28);
	NRF_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);    
	NRF_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);  
	NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x31);
	SPI_CE_H;
}

void nrf_TXMode(void)
{
	SPI_CE_L;  
	NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x42);     // Disable Rx interrupt, turn to Tx mode
	NRF_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	NRF_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // For auto axknowledge
	
	NRF_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);      // Enabled pipe0 auto ack 
	NRF_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  // Enabled pipe0 RX address
	NRF_Write_Reg(NRF_WRITE_REG + SETUP_AW, 0x05);   // Setup of address widths
	NRF_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1a); // Setup of automatic retransmission: 500us, 10times
	NRF_Write_Reg(NRF_WRITE_REG + RF_CH, 0x28);      // Set the frequency channel
	NRF_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x26);   // Set data rate and output power:0db, 1Mbps
	
	SPI_CE_H; // A high pulse on CE starts the transmission
	delay_us(20);
}

void nrfSlaveMode(void)
{
	nrf_RXMode();
}

void nrfMasterMode(void)
{
	nrf_TXMode();
}
	
uint8_t nrfIqrTx_Packet(uint8_t *txbuffter)
{
	SPI_CE_L;		
	NRF_Write_Buf(WR_TX_PLOAD,txbuffter,TX_PLOAD_WIDTH);
 	SPI_CE_H;
	return 1;
}


