#include "stm32f10x.h"
#include "string.h"

#include "systeminit.h"
#include "delay.h"
#include "system_config.h"
//#include "nrf24app.h"
#include "spinrf.h"

#ifdef _USBDEBUG_
	#include "usb2com.h"
	#include "debug.h"
#else
	#include "usart.h"
#endif

uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH];//nrf24l01接收到的数据
uint8_t NRF24L01_TXDATA[TX_PLOAD_WIDTH];//nrf24l01需要发送的数据
uint8_t NrfState;

#define RT_TIMES 10.0

uint8_t  RX_ADDRESS[RX_ADR_WIDTH]= {0x35,0xc3,0x10,0x10,0x00};	//本机地址	
uint8_t  TX_ADDRESS[TX_ADR_WIDTH]= {0x35,0xc3,0x10,0x10,0x11};	//接收地址

#define TX_MODE 0
#define RX_MODE 1
#define TRANS_TO_TX 2
#define TRANS_TO_RX 3
#define LOSTCONTROLTRIGGER 5
#define TIMEOUT 1429

#define RXFAILTRIGGER 2
uint32_t timeCount = 0;
uint8_t txFailCount = 0, rxFailCount = 0;
uint8_t lostControlFlag = 0, rxFailWarning = 0;

typedef struct 
{
	uint8_t head[2];
	uint16_t thrust;
	float roll;
	float pitch;
	float yaw;
		
	uint8_t motor1;
	uint8_t motor2;
	uint8_t motor3;
	uint8_t motor4;
	uint16_t battery;
	uint8_t linkQuality;
	uint8_t resevered[1];
		
	float broVal;
	uint8_t haveBro;
	uint8_t connect;
	uint8_t tail[2];
}RespondMess;
	
RespondMess BBMess;

int main(void)
{
	uint32_t i;

	sysInit();
	spi1NrfInit();
	
	if(NRF24L01_Check() == 1)
	{
		//USB_printf("NRFCHECK OK");
	}

	usbVirtualComInit();
	
	NrfState = TRANS_TO_TX;
	while(1)
	{	
		if(lostControlFlag == 1)
		{
			//LedD_on;
		}
		else
		{
			//LedD_off;
		}
		
	    switch(NrfState)
		{
			case TRANS_TO_TX: SPI_CE_L;  
				              NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x4A);     // Disable Rx interrupt, turn to Tx mode
							  NRF_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
							  NRF_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // For auto acknowledge
								
						      NRF_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);      // Enabled pipe0 auto ack 
							  NRF_Write_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  // Enabled pipe0 RX address
							  NRF_Write_Reg(NRF_WRITE_REG + SETUP_AW, 0x05);   // Setup of address widths
							  NRF_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1f); // Setup of automatic retransmission: 500us, 15times
							  NRF_Write_Reg(NRF_WRITE_REG + RF_CH, 0x5e);      // Set the frequency channel
							  NRF_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x26);   // Set data rate and output power:0db, 2Mbps (0x26 0dm 250kbps)
							  
							  //if(rxFailWarning != 1)
							  //{
								  i = USB_RxRead(NRF24L01_TXDATA, TX_PLOAD_WIDTH);
								  if(i == 0)
								  {
									  for(i=0; i<TX_PLOAD_WIDTH; i++)
									  {
										  NRF24L01_TXDATA[i] = 0x00;
									  }
								  }	
							  //}								  

							  NRF_Write_Buf(WR_TX_PLOAD, NRF24L01_TXDATA, TX_PLOAD_WIDTH);
								
							  NrfState = TX_MODE;					  
							  SPI_CE_H; // A high pulse on CE starts the transmission
							  break;
									  
			case TRANS_TO_RX: SPI_CE_L;
				              NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x3B);
							  NRF_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);
								  
							  NRF_Write_Reg(NRF_WRITE_REG + EN_AA, 0x01);
							  NRF_Write_Reg(NRF_WRITE_REG + EN_RXADDR,0x01);
							  NRF_Write_Reg(NRF_WRITE_REG + SETUP_AW, 0x05);
							  NRF_Write_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1f);
							  NRF_Write_Reg(NRF_WRITE_REG + RF_CH, 0x5e);
							  NRF_Write_Reg(NRF_WRITE_REG + RF_SETUP, 0x26);  //26
								  
							  NRF_Write_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);    
				
			                  NrfState = RX_MODE;				  
							  SPI_CE_H;
							  break;
												  
			case TX_MODE: timeCount = 0;
						  while((NrfState == TX_MODE) && (timeCount < TIMEOUT*1500))
						  {
							  timeCount ++;
						  }
						  NrfState = TRANS_TO_RX;
						  break;
			case RX_MODE: timeCount = 0;
						  while((NrfState == RX_MODE) && (timeCount < TIMEOUT*1500))
						  {
							  timeCount ++;
						  }
						  NrfState = TRANS_TO_TX; // Time out  
						  if(timeCount < TIMEOUT*1500)
						  {
							delay_ms(200); //MMMMMMMMAAAAAAAAAAAAAARRRRRRRRRRRRRKKKKKKKKKK
						  }
						  /*if(timeCount >= TIMEOUT*1500)
						  {
							  rxFailCount ++;
							  if(rxFailCount > RXFAILTRIGGER)
							  {
								  rxFailWarning = 1;
							  }
						  }
						  else
						  {
							  rxFailCount = 0;
							  rxFailWarning = 0;
						  }*/ 
				          break;
		}				
	}			
}

void nrfTransmitResult(uint8_t res, uint8_t linkQuality)
{
	if(res == 0)
	{
		txFailCount ++;
		delay_noInt_ms(100);
	}
	else
	{
		txFailCount = 0;
		lostControlFlag = 0;
	}
	if(txFailCount > LOSTCONTROLTRIGGER)
	{
		lostControlFlag = 1;
	}
}

void nrfReceiveResult(uint8_t res)
{
	if(res == 1)
	{
		if((NRF24L01_RXDATA[0] == 0xaa)&&(NRF24L01_RXDATA[31] == 0xff))
		{
			USB_TxWrite(NRF24L01_RXDATA, RX_PLOAD_WIDTH);
		}
	}
}

void EXTI3_IRQHandler(void)
{
	uint8_t state;
	uint8_t linkQuality;
	switch(NrfState)
	{
		case TX_MODE:   SPI_CE_L;
							
				        state = NRF_Read_Reg(NRF_STATUS);  
						linkQuality = (NRF_Read_Reg(OBSERVE_TX) &0xf0 >>4) /RT_TIMES *100.0;
						NRF_Write_Reg(NRF_WRITE_REG +NRF_STATUS,state); 	
						NRF_Write_Reg(FLUSH_TX,NOP);
			
						if(state &MAX_RT)
						{
							nrfTransmitResult(0, linkQuality); 
						}
						else if(state &TX_DS)
						{	
							nrfTransmitResult(1, linkQuality); 
						}
							
						NrfState = TRANS_TO_RX;	
						EXTI_ClearITPendingBit(EXTI_Line3);

						break;
			
		case RX_MODE:   SPI_CE_L;
						state = NRF_Read_Reg(NRF_STATUS); 		 
						NRF_Write_Reg(NRF_WRITE_REG + NRF_STATUS,state);
						if(state &RX_DR)
						{
							NRF_Read_Buf(RD_RX_PLOAD, NRF24L01_RXDATA, RX_PLOAD_WIDTH);
							
							NRF_Write_Reg(FLUSH_RX,NOP);
							nrfReceiveResult(1);
						}	
						NrfState = TRANS_TO_TX;
						EXTI_ClearITPendingBit(EXTI_Line3);

						break;
	}			
	
}


