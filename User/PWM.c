//
// Created by 86180 on 2024/7/17.
//
// 使用PWM，周期设置为3MHz，发送0就把占空比设置为33%，发送1就把占空比设置为66%

#include "PWM.h"
#define TIM2_CH2CVR_ADDRESS    0x40000038

/* PWM Output Mode Definition */
#define TIMING_ONE  40
#define TIMING_ZERO 18

uint32_t pbuf[300];
//功能：用于打开PWM，发出数据使灯亮起来

void WS_Send(uint8_t *color,uint16_t len){
    uint8_t  i;
    uint16_t memaddr;
    uint16_t buffersize;
    buffersize = (len * 24) + 43;
    memaddr = 0;

    while (len){
        for (i = 0; i < 8; ++i) {//green
            pbuf[memaddr] = ((color[1]<<i)& 0x0080)?TIMING_ONE:TIMING_ZERO;
            memaddr ++;
        }
        for (i = 0; i < 8; ++i) {//red
            pbuf[memaddr] = ((color[0]<<i)& 0x0080)?TIMING_ONE:TIMING_ZERO;
            memaddr ++;
        }
        for (i = 0; i < 8; ++i) {//blue
            pbuf[memaddr] = ((color[2]<<i)& 0x0080)?TIMING_ONE:TIMING_ZERO;
            memaddr ++;
        }
        len --;
    }
//    pbuf[memaddr] = ((color[0][2]<<8))?TIMING_ONE:TIMING_ZERO;
//    TIM2_DMA_Init(DMA1_Channel5,(u32)TIM2_CH2CVR_ADDRESS,(u32 )pbuf,memaddr);
    TIM2_DMA_Init(DMA1_Channel2, (u32)TIM2_CH2CVR_ADDRESS, (u32)pbuf, memaddr);
}

void TIM2_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM2,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
}
void TIM2_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    DMA_Cmd(DMA_CHx, ENABLE);
}

/*
 * A4 A5 A6 B0 B1 五个键盘开关
 * B1 大按钮
 * B2 B3 B4 三个键盘开关
 * B5 继电器
 * A1 5个灯
 * A7 3个灯
 * B8 usb路径切换

 */



