//
// Created by 86180 on 2024/7/17.
//

#ifndef L103PRO_USER_PWM_H_
#define L103PRO_USER_PWM_H_
#include "debug.h"
#include "ch32l103.h"
#define CODE_1  7
#define CODE_0 3
#define RESET_LEN    (60)
#define  Pixel_NUM 8
#define COLOR_BUFFER_LEN (((Pixel_NUM)*(3*8))+RESET_LEN)
#define MAX_STEP (200)
#define LIST_SIZE(list) (sizeof(list)/sizeof(list[0]))
#define hex2rgb(c) (((c)>>16)&0xff),(((c)>>8)&0xff),((c)&0xff)


void TIM2_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void TIM2_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u16 bufsize);
void WS_Send(uint8_t  *color,uint16_t len);
void setPixelColor(uint16_t id, uint8_t r, uint8_t g, uint8_t b);
void w2812_sync();
uint32_t interpolateColors(uint32_t color1, uint32_t color2, uint8_t step);
void led_example_0(void);
void led_example_2(void);



#endif //L103PRO_USER_PWM_H_
