//
// Created by 86180 on 2024/6/5.
//
#ifndef __INA226_H
#define __INA226_H
//#include "system_ch32l103.h"
#include "debug.h"

#define addr1                      0x80
#define addr2                      0x82
#define addr3                      0x84
#define addr4                      0x86
#define addr5                      0x88
#define addr6                      0x80
#define addr7                      0x82
#define addr8                      0x84
#define Config_Reg                 0x00
#define Shunt_V_Reg                0x01//并联电压
#define Bus_V_Reg                  0x02//测量电压
#define PASSED                       0
#define FALSE                      1

#define Power_Reg                  0x03//功率寄存器，值等 于 当 前 LSB编 程 值 的 25倍
#define Current_Reg                0x04
#define Calib_Reg                  0x05
#define Mask_En_Reg                0x06//控制使能警报，选用警报选项为电流
#define Alert_Reg_limit            0x07//警报
#define Man_ID_Reg                 0xFE  //0x5449
#define ID_Reg                     0xFF  //0x2260

#define config_vlaue               0x4127//0100_010_100_100_111 //采集16次取平均值更新一次,连续测量分流电压和总线电压
#define calibrate_value            0x3200//12800

__attribute__((unused)) u8 INA226_ReadByte(u8 device_addr,u8 reg_addr);
u8 INA226_Read2Byte_I2C1(u8 device_addr, u8 reg_addr, u16 *data)  ;
u8 INA226_Read2Byte_I2C2(u8 device_addr, u8 reg_addr, u16 *data) ;

__attribute__((unused)) void INA226_WriteByte(u8 device_addr,u8 reg_addr,u8 reg_data);
void INA226_Write2Byte_I2C1(u8 device_addr, u8 reg_addr, u16 reg_data) ;
void INA226_Write2Byte_I2C2(u8 device_addr, u8 reg_addr, u16 reg_data) ;
void INA226_Init(void);
void IIC_Init(u32 bound) ;

void delay(int time_);

#endif
