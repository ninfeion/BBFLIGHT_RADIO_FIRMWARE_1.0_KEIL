#ifndef _SPINRF_H_
#define _SPINRF_H_

#include "stm32f10x.h"
#include "stm32f10x_exti.h"

#define SPI_CE_H    		GPIO_SetBits(GPIOB, GPIO_Pin_3) 
#define SPI_CE_L  		    GPIO_ResetBits(GPIOB, GPIO_Pin_3)

#define SPI_CSN_H 	 	    GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SPI_CSN_L  		    GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define NRF_Read_IRQ()	    GPIO_ReadInputDataBit ( GPIOA, GPIO_Pin_3) //�ж�����

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
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define NRF_READ_REG    0x00  	// ���Ĵ���ָ��
#define NRF_WRITE_REG   0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ��������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define NRF_STATUS      0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // Transmit observe register: 7:4:count lost packets, max in 15, shoule reset by writing to RF_CH 
							  // 3:0:count retransmitted packets, counter reset when tx now packet start

#define RPD             0x09  // Received Power Detector          
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ 5 Bytes length
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ 5 Bytes length
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ 39:8 equal to RX_ADDR_P1
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ 39:8 equal to RX_ADDR_P1
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ 39:8 equal to RX_ADDR_P1
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ 39:8 equal to RX_ADDR_P1
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������

#define RX_DR			0x40  //�жϱ�־
#define TX_DS			0x20
#define MAX_RT			0x10

#define NOTFINISHLASTTX 0xFF  //��һ��û���

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

//extern uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH];//nrf24l01���յ�������
//extern uint8_t NRF24L01_TXDATA[RX_PLOAD_WIDTH];//nrf24l01��Ҫ���͵�����
//extern uint8_t NRF_TX_FINISH_state;

#endif