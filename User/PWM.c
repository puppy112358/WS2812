//
// Created by 86180 on 2024/7/17.
//

#include "PWM.h"

/* PWM Output Mode Definition */
uint16_t color_buf[COLOR_BUFFER_LEN] = {0};
void setPixelColor(uint16_t id, uint8_t r, uint8_t g, uint8_t b)
{
    int i = 0, j = id * 24u;
    if (id >= Pixel_NUM) {
        return;
    }

    for (i = 0; i < 8; i++) {
        if (g & (0x80 >> i)) {
            color_buf[j] = CODE_1;
        } else {
            color_buf[j] = CODE_0;
        }
        j++;
    }
    for (i = 0; i < 8; i++) {
        if (r & (0x80 >> i)) {
            color_buf[j] = CODE_1;
        } else {
            color_buf[j] = CODE_0;
        }
        j++;
    }
    for (i = 0; i < 8; i++) {
        if (b & (0x80 >> i)) {
            color_buf[j] = CODE_1;
        } else {
            color_buf[j] = CODE_0;
        }
        j++;
    }
}
void w2812_sync()
{
    while(DMA_GetCurrDataCounter(DMA1_Channel2)!=0);
    DMA_SetCurrDataCounter( DMA1_Channel2, COLOR_BUFFER_LEN);
    DMA_Cmd( DMA1_Channel2, ENABLE);
    TIM_Cmd( TIM2, ENABLE);
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
void TIM2_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_Cmd(DMA_CHx,DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t )color_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    DMA_Cmd(DMA_CHx, ENABLE);

    DMA_ITConfig( DMA1_Channel2, DMA_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
uint32_t interpolateColors(uint32_t color1, uint32_t color2, uint8_t step)
{
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;

    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;

    float tmp1 = (r2 - r1) * (step / (float) MAX_STEP);
    float tmp2 = (g2 - g1) * (step / (float) MAX_STEP);
    float tmp3 = (b2 - b1) * (step / (float) MAX_STEP);

    uint8_t r = (uint8_t) (r1 + tmp1);
    uint8_t g = (uint8_t) (g1 + tmp2);
    uint8_t b = (uint8_t) (b1 + tmp3);

    return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
}

uint32_t color_list[] = {
    0xff0000,
    0x00ff00,
    0x0000ff,
    0xffff00,
    0xff00ff,
    0x00ffff,
    0xffffff,
//        0x000000,
};
/*********************************************************************
 * @fn      led_example_0
 *
 * @brief   In this example, all leds will gradually change from one color to another
 *
 * @return  none
 */
void led_example_0(void)
{
    uint32_t i = 0, j = 0;
    uint32_t c = 0x0f0f00;
    while(1) {
        c = color_list[i];
        uint32_t next_color = color_list[i+1];
        i++;
        if((i+1)>=LIST_SIZE(color_list)) {
            i=0;
        }
        for(j = 0; j<MAX_STEP;j+=1) {
            uint32_t color = interpolateColors(c,next_color,j);
            for (int var = 0; var < Pixel_NUM; ++var) {
                setPixelColor(var,hex2rgb(color));

            }
            w2812_sync();
            Delay_Ms(10);

        }

    }
}

/*********************************************************************
 * @fn      led_example_2
 *
 * @brief   In this example, the leds will blink one by one
 *
 * @return  none
 */
void led_example_2(void)
{
    uint32_t i = 0;
    uint32_t c = 0x0f0f00;
    while(1) {
        c = color_list[i];
        printf("color[%d] %08x  %08x \n",i,c,color_list[i]);
        i++;
        if(i>=LIST_SIZE(color_list)) {
            i=0;
        }

        for (int var = 0; var < Pixel_NUM; var+=1) {
            setPixelColor(var, hex2rgb(c));
            w2812_sync();
            Delay_Ms(100);
            setPixelColor(var,0,0,0);
            w2812_sync();
            Delay_Ms(10);
        }
    }
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



