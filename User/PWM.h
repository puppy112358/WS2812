//
// Created by 86180 on 2024/7/17.
//

#ifndef L103PRO_USER_PWM_H_
#define L103PRO_USER_PWM_H_
#include "debug.h"
void TIM2_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void TIM2_DMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
void WS_Send(uint8_t *color,uint16_t len);

#endif //L103PRO_USER_PWM_H_
