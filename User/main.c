/*
 *I2C1_SCL(PB10)\I2C1_SDA(PB11).
 */

#include <string.h>
#include "debug.h"
#include "PWM.h"
#define Address_8bit  0
#define Address_16bit  1
/* CH1CVR register Definition */
#define TIM2_CH2CVR_ADDRESS    0x40000038

/* Private variables */

#define Address_Lenth   Address_8bit
//#define Address_Lenth   Address_16bit


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 *
 * @return  none
 */
int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    uint8_t led = 0;
    printf("SystemClk:%lu\r\n", SystemCoreClock);
    printf("ChipID:%08lx\r\n", DBGMCU_GetCHIPID());

//GPIO 初始化
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    uint32_t p[3] = {10,20,30};
    TIM2_PWMOut_Init(60-1, 2-1, 10);
//    TIM2_DMA_Init(DMA1_Channel2, (u32)TIM2_CH2CVR_ADDRESS, (u32)p, 3);
//
    TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);

    uint8_t color_red[] = {255, 0, 0};
    WS_Send(color_red, 63);

    while (1) {
        //闪灯确认工作状态
        if (led == 1) {
            led = 0;
        } else {
            led = 1;
        }
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, led);

        Delay_Ms(100);
    }

}


