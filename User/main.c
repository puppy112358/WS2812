/*
 *I2C1_SCL(PB10)\I2C1_SDA(PB11).
 */

#include <string.h>
#include "debug.h"
#include "ina226.h"
#include "uart.h"
#include "TJCScreen.h"
#include "PWM.h"
#define Address_8bit  0
#define Address_16bit  1

/* Global Variable */
uint8_t RxBuffer1[6] = {0};                                             /* USART2 Using */
int dianliu_yuzhi[8] = {0};
float Bus_V[8] = {0};
float current[8] = {0};
float power[8] = {0};

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


/* EERPOM DATA ADDRESS Length Selection */
#define Address_Lenth   Address_8bit
//#define Address_Lenth   Address_16bit

#define  kaiguan1 GPIO_WriteBit(GPIOB, GPIO_Pin_12, RxBuffer1[2]);
#define  kaiguan2 GPIO_WriteBit(GPIOB, GPIO_Pin_13, RxBuffer1[2]);
#define  kaiguan3 GPIO_WriteBit(GPIOB, GPIO_Pin_14, RxBuffer1[2]);
#define  kaiguan4 GPIO_WriteBit(GPIOB, GPIO_Pin_15, RxBuffer1[2]);
#define  kaiguan5 GPIO_WriteBit(GPIOA, GPIO_Pin_8 , RxBuffer1[2]);
#define  kaiguan6 GPIO_WriteBit(GPIOA, GPIO_Pin_11, RxBuffer1[2]);
#define  kaiguan7 GPIO_WriteBit(GPIOA, GPIO_Pin_12, RxBuffer1[2]);
#define  kaiguan8 GPIO_WriteBit(GPIOA, GPIO_Pin_15, RxBuffer1[2]);


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 *
 * @return  none
 */
int main(void) {
/*
 * 上限为 5A  5V
 *  ???????
 *  ????LSB = ????????? / 2 ^ 15       0.0001885

    У??????=0.00512/????LSB/0.002      12800  3200
    ???????? = ??????????*????LSB = ?????????????*У???????/2048
 *  ?????? = ????????????*1.25
 *  ???? = ?????????*25*????LSB=????*??????
 * */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    DMA_INIT();
    USARTx_CFG();
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
    TIM2_PWMOut_Init( 100, 48000-1, 50 );

    TJCScreenInit(USART2);

    char Screen_Txt[20];
    char Index[3];
    uint8_t led = 0;
    u16 temp = 0;
    printf("SystemClk:%lu\r\n", SystemCoreClock);
    printf("ChipID:%08lx\r\n", DBGMCU_GetCHIPID());
    int j = 0;
    INA226_Init();
    // 串口屏初始化
    for (int i = 0; i < 7; i++) {
        sprintf(Index, "p%d", i);
        TCJSetPic(Index, i);
    }
    sprintf(Screen_Txt, "电压：%.4fV",0.0000);
    for (int i = 1; i < 50; i += 7) {
        sprintf(Index, "t%d", i);
        TCJSendTxt(Index, Screen_Txt);
    }
    sprintf(Screen_Txt, "电流：%.4fA",0.0000);
    for (int i = 0; i < 51; i += 7) {
        sprintf(Index, "t%d", i);
        TCJSendTxt(Index, Screen_Txt);
    }
    sprintf(Screen_Txt, "功率：%.4fW",0.0000);
    for (int i = 2; i < 52; i += 7) {
        sprintf(Index, "t%d", i);
        TCJSendTxt(Index, Screen_Txt);
    }
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

    while (1) {
        //闪灯确认工作状态
        if (led == 1) {
            led = 0;
        } else {
            led = 1;
        }
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, led);

        Delay_Ms(1);
        // 打印电压值*
        j = 0;
        for (int i = 1; i < 51; i += 7) {
            sprintf(Screen_Txt, "电压：%.4fV", Bus_V[j]);
//            printf("%d   电压%.4f",j,Bus_V[j]);
            sprintf(Index, "t%d", i);
            TCJSendTxt(Index, Screen_Txt);
            j++;
        }
        // 打印电流值
        j = 0;
        for (int i = 0; i < 51; i += 7) {
            sprintf(Screen_Txt, "电流：%.4fA", current[j]);
//            printf("%d   电流%.4f\r\n",j,current[j]);
            sprintf(Index, "t%d", i);
            TCJSendTxt(Index, Screen_Txt);
            j++;
        }
        // 打印功率值
        j = 0;
        for (int i = 2; i < 52; i += 7) {
            sprintf(Screen_Txt, "功率：%.4fW", power[j]);
//            sprintf(Index, "t%d", i);
            TCJSendTxt(Index, Screen_Txt);
            j++;
        }

//        计算电压
        if (INA226_Read2Byte_I2C1(addr1, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[0] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C1(addr2, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[1] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C1(addr3, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[2] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C1(addr4, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[3] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C1(addr5, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[4] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C2(addr6, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[5] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C2(addr7, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[6] = temp * (double) 0.00125;
        }
        if (INA226_Read2Byte_I2C2(addr8, Bus_V_Reg, &temp) == PASSED) {
            Bus_V[7] = temp * (double) 0.00125;
        }
//计算电流
//没接入设备时电流为0
        if (Bus_V[0] > 1) {
            INA226_Read2Byte_I2C1(addr1, Current_Reg, &temp);
            if (temp < 65000) {
                current[0] = temp * 0.0001885;
            }
        } else {
            current[0] = 0;
        }
        if (Bus_V[1] > 1) {
            INA226_Read2Byte_I2C1(addr2, Current_Reg, &temp);
            if (temp < 65000) {
                current[1] = temp * 0.0001885;
            }
        } else {
            current[1] = 0;
        }
        if (Bus_V[2] > 1) {
            INA226_Read2Byte_I2C1(addr3, Current_Reg, &temp);
            if (temp < 65000) {
                current[2] = temp * 0.0001885;
            }
        } else {
            current[2] = 0;
        }
        if (Bus_V[3] > 1) {
            INA226_Read2Byte_I2C1(addr4, Current_Reg, &temp);
            if (temp < 65000) {
                current[3] = temp * 0.0001885;
            }
        } else {
            current[3] = 0;
        }
        if (Bus_V[4] > 1) {
            INA226_Read2Byte_I2C1(addr5, Current_Reg, &temp);
            if (temp < 65000) {
                current[4] = temp * 0.0001885;
            }
        } else {
            current[4] = 0;
        }
        if (Bus_V[5] > 1) {
            INA226_Read2Byte_I2C2(addr6, Current_Reg, &temp);
            if (temp < 65000) {
                current[5] = temp * 0.0001885;
            }
        } else {
            current[5] = 0;
        }
        if (Bus_V[6] > 1) {
            INA226_Read2Byte_I2C2(addr7, Current_Reg, &temp);
            if (temp < 65000) {
                current[6] = temp * 0.0001885;
            }
        } else {
            current[6] = 0;
        }
        if (Bus_V[7] > 1) {
            INA226_Read2Byte_I2C2(addr8, Current_Reg, &temp);
            if (temp < 65000) {
                current[7] = temp * 0.0001885;
            }
        } else {
            current[7] = 0;
        }
//        //计算功率
        power[0] = Bus_V[0] * current[0];
        power[1] = Bus_V[1] * current[1];
        power[2] = Bus_V[2] * current[2];
        power[3] = Bus_V[3] * current[3];
        power[4] = Bus_V[4] * current[4];
        power[5] = Bus_V[5] * current[5];
        power[6] = Bus_V[6] * current[6];
        power[7] = Bus_V[7] * current[7];
        if (RxBuffer1[0] != 0) {
            switch (RxBuffer1[0]) {
                case kaiguan:
                    switch (RxBuffer1[1]) {
                        case 1:
                            kaiguan1;
                            break;
                        case 2:
                            kaiguan2;
                            break;
                        case 3:
                            kaiguan3;
                            break;
                        case 4:
                            kaiguan4;
                            break;
                        case 5:
                            kaiguan5;
                            break;
                        case 6:
                            kaiguan6
                            break;
                        case 7:
                            kaiguan7;
                            break;
                        case 8:
                            kaiguan8;
                            break;
                        default:
                            break;
                    }
                    break;
                case dianliu:
                    dianliu_yuzhi[RxBuffer1[1] - 1] = RxBuffer1[2];
                    dianliu_yuzhi[RxBuffer1[1] - 1] = dianliu_yuzhi[RxBuffer1[1] - 1] | (RxBuffer1[3] << 4);
                    switch (RxBuffer1[1]) {////TODO: 电流阈值分度值
                        case 1:
                            INA226_Write2Byte_I2C1(addr1, Alert_Reg_limit, dianliu_yuzhi[0] * 50);
                            break;
                        case 2:
                            INA226_Write2Byte_I2C1(addr2, Alert_Reg_limit, dianliu_yuzhi[1] * 50);
                            break;
                        case 3:
                            INA226_Write2Byte_I2C1(addr3, Alert_Reg_limit, dianliu_yuzhi[2] * 50);
                            break;
                        case 4:
                            INA226_Write2Byte_I2C1(addr4, Alert_Reg_limit, dianliu_yuzhi[3] * 50);
                            break;
                        case 5:
                            INA226_Write2Byte_I2C1(addr5, Alert_Reg_limit, dianliu_yuzhi[4] * 50);
                            break;
                        case 6:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[5] * 50);
                            break;
                        case 7:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[6] * 50);
                            break;
                        case 8:
                            INA226_Write2Byte_I2C2(addr6, Alert_Reg_limit, dianliu_yuzhi[7] * 50);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

    }

}


void USART2_IRQHandler(void)
{
    uint8_t temp[10] = {0};
    if(USART_GetITStatus(USART2,USART_IT_IDLE) !=RESET){//空闲中断
        DMA_Cmd(DMA1_Channel6,DISABLE);
        memcpy(temp,RxBuffer1,10);
        USART_Send(USART1,temp,10);
        memset(RxBuffer1,0,sizeof(RxBuffer1));
        memset(temp,0,sizeof(temp));
        USART_ClearITPendingBit(USART2,USART_IT_IDLE);
        DMA_SetCurrDataCounter( DMA1_Channel6, 10 );
        DMA_Cmd(DMA1_Channel6,ENABLE);//
        USART_ReceiveData(USART2);//
    }
}
