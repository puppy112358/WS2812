#include <string.h>
#include "uart.h"
/*******************************************************************************
  * @函数名称	USART_Send
  * @函数说明   发送信息
  * @输入参数   _UART：串口号
								data：要发送的信息的首地址
                len： 发送的长度
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void USART_Send(USART_TypeDef *_UART,u8 *data, u8 len)
{
    for(int i = 0; i < len; i++)
    {
        USART_SendData(_UART, data[i]); //向串口发送数据
        while(USART_GetFlagStatus(_UART, USART_FLAG_TXE) == RESET); //等待发送结束
    }
}
/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2  peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_USART2, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA , ENABLE);

    /* USART2 TX-->A.2   RX-->A.3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel6,ENABLE);//USART2->RX
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//空闲中断
//    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);

    USART_Cmd(USART2, ENABLE);
}
/*********************************************************************
 * @fn      DMA_INIT
 *
 * @brief   Configures the DMA for USART2 & USART3.
 *
 * @return  none
 */
void DMA_INIT(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxBuffer1;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = strlen(RxBuffer1);
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

}



