
#include "debug.h"
#include "PWM.h"
#define Address_8bit  0
#define Address_16bit  1
#define TIM2_CH2CVR_ADDRESS    0x40000038

/* Private variables */

#define Address_Lenth   Address_8bit
extern uint16_t color_buf[COLOR_BUFFER_LEN];

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
    printf("SystemClk:%lu\r\n", SystemCoreClock);
    printf("ChipID:%08lx\r\n", DBGMCU_GetCHIPID());

//GPIO ≥ı ºªØ
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
    TIM2_PWMOut_Init(10-1, 12-1, 0);
    TIM2_DMA_Init(DMA1_Channel2, (u32)TIM2_CH2CVR_ADDRESS, COLOR_BUFFER_LEN);

    TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    for (int i = 0; i < Pixel_NUM; i++) {
        setPixelColor(i, 0, 0, 0);
    }
    led_example_2();


}
void DMA1_Channel2_IRQHandler(void)
{
    if (DMA_GetFlagStatus( DMA1_FLAG_TC2)) {
        TIM_Cmd( TIM2, DISABLE);
        DMA_Cmd( DMA1_Channel2, DISABLE);
        DMA_ClearFlag( DMA1_FLAG_TC2);
    }
}


