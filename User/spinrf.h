#ifndef _SPINRF_H_
#define _SPINRF_H_

#include "stm32f10x.h"
#include "stm32f10x_exti.h"

#define SPI_CE_H    		GPIO_SetBits(GPIOB, GPIO_Pin_3) 
#define SPI_CE_L  		    GPIO_ResetBits(GPIOB, GPIO_Pin_3)

#define SPI_CSN_H 	 	    GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SPI_CSN_L  		    GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define NRF_Read_IRQ()	    GPIO_ReadInputDataBit ( GPIOA, GPIO_Pin_3) //中断引脚

#define NRF_CE_PIN  		GPIO_Pin_3
#define NRF_CE_PORT 		GPIOB

#define NRF_CSN_PIN         GPIO_Pin_4
#define NRF_CSN_PORT  	    GPIOA

#define NRF_SCK_PIN 		GPIO_Pin_5
#define NRF_SCK_PORT 		GPIOA
#define NRF_SCK_PERIF		RCC_APB2Periph_GPIOA

#define NRF_MISO_PIN		GPIO_Pin_6
#define NRF_MISO_PORT 	    GPIOA

#define NRF_MOSI_PIN        GPIO_Pin_7
#define NRF_MOSI_PORT 	    GPIOA

#define NRF_IRQ_PIN  		GPIO_Pin_3
#define NRF_IRQ_PORT		GPIOA

#define SPI_X 				SPI1
#define SPI_PERIF			RCC_APB2Periph_SPI1

//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width

#define RX_PLOAD_WIDTH  32  	// 32 uints TX payload
#define TX_PLOAD_WIDTH  32  	// 32 uints TX payload
//***************************************NRF24L01寄存器指令*******************************************************
#define NRF_READ_REG    0x00  	// 读寄存器指令
#define NRF_WRITE_REG   0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define NRF_STATUS      0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // Transmit observe register: 7:4:count lost packets, max in 15, shoule reset by writing to RF_CH 
							  // 3:0:count retransmitted packets, counter reset when tx now packet start

#define RPD             0x09  // Received Power Detector          
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址 5 Bytes length
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址 5 Bytes length
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址 39:8 equal to RX_ADDR_P1
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址 39:8 equal to RX_ADDR_P1
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址 39:8 equal to RX_ADDR_P1
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址 39:8 equal to RX_ADDR_P1
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道1接收数据长度
#define RX_PW_P2        0x13  // 接收频道2接收数据长度
#define RX_PW_P3        0x14  // 接收频道3接收数据长度
#define RX_PW_P4        0x15  // 接收频道4接收数据长度
#define RX_PW_P5        0x16  // 接收频道5接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置

#define RX_DR			0x40  //中断标志
#define TX_DS			0x20
#define MAX_RT			0x10

#define NOTFINISHLASTTX 0xFF  //上一次没完成

uint8_t spi1NrfInit(void);
uint8_t SPI_RW(uint8_t dat);
uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value);
uint8_t NRF_Read_Reg(uint8_t reg);
uint8_t NRF24L01_Check(void); 

uint8_t NRF24L01_RxPacket(uint8_t *rxbuf);
uint8_t NRF_TxPacket(uint8_t *tx_buf);
uint8_t nrfIqrTx_Packet(uint8_t *txbuffter);

void nrf_RXMode(void);
void nrf_TXMode(void);
void nrfSlaveMode(void);
void nrfMasterMode(void);

//extern uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH];//nrf24l01接收到的数据
//extern uint8_t NRF24L01_TXDATA[RX_PLOAD_WIDTH];//nrf24l01需要发送的数据
//extern uint8_t NRF_TX_FINISH_state;

#endif
