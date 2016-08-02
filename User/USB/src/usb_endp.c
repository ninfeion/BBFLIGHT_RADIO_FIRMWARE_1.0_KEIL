/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"



/*���ｲ�´����˼������EP3_OUT_Callback����USB���յ����ݺ󣬽����ݴ���FIFO�С�
����SOF_Callback��ʱ��ѯ�û��Ƿ���Ҫ���͵����ݣ����������з��ͣ�
�ڷ�����ɺ�ᴥ�������ж�EP1_IN_Callback������
���������ϾͲ�����SetEPTxValid(ENDP1)������
������ɺ�Ͳ����ٴ���EP1_IN_Callback������*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t txBuffter[VIRTUAL_COM_PORT_DATA_SIZE] = {0};
static volatile uint8_t txFlg = 0;
static volatile uint32_t FrameCount = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
    uint16_t len = 0;
    
    if (1 == txFlg)
    {
        len = USB_TxRead(txBuffter, sizeof(txBuffter));

        if (len > 0)
        {
            UserToPMABufferCopy(txBuffter, ENDP1_TXADDR, len);
            SetEPTxCount(ENDP1, len);
            SetEPTxValid(ENDP1); 
            FrameCount = 0;
        }
        else
        {
            txFlg = 0;
        }
    }
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  static uint8_t buffter[VIRTUAL_COM_PORT_DATA_SIZE] = {0};

  uint16_t USB_Rx_Cnt;
  
  /* Get the received data buffer and update the counter */
  USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, buffter);
  
  /* USB data will be immediately processed, this allow next USB traffic being 
  NAKed till the end of the USART Xfer */
  USB_RxWrite(buffter, USB_Rx_Cnt);

  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);

}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
    uint16_t len = 0;

    if(bDeviceState == CONFIGURED)
    {
        if (0 == txFlg)
        {
            if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
            {
                /* Reset the frame counter */
                FrameCount = 0;

                /* Check the data to be sent through IN pipe */
                len = USB_TxRead(txBuffter, sizeof(txBuffter));

                if (len > 0)
                {
                    UserToPMABufferCopy(txBuffter, ENDP1_TXADDR, len);
                    SetEPTxCount(ENDP1, len);
                    SetEPTxValid(ENDP1);

                    txFlg = 1;
                }
            }
        }
    }  
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

