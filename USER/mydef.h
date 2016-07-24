/********************** 移植时要特别关注的 *******************************
* 作者：刘帅
* 日期：2015/10/31
* 修改：2016/01/10
* 修改：2016/04/01  add USB
* 修改：2016/07/20  更改部分函数名 & 更正缩进
* 修改：2016/07/23  加入通道开关宏等
************************************************************************/
#ifndef __MYDEF_H
#define __MYDEF_H

#include <stdarg.h>
#include <stdint.h>

#define VCC_V                           3.3                         //float  取样电压最大值
#define ADC_B                           12                          //int     ADC位数

//是否使用虚拟串口
#define USE_USB_VIRTUAL_COM             0                           //0 使用硬件串口  1 使用USB虚拟的串口

//上传数据
#define PRINTF_LABVIEW(format, ...)     printf(format, ##__VA_ARGS__)

#define num_of_data 1024

#define USE_CHANNEL_C1                  1
#define USE_CHANNEL_C2                  1
#define USE_CHANNEL_C3                  1

//触发中断允许
#define EXTI_ENABLE                     EXTI_Control(ENABLE)
//触发中断禁止
#define EXTI_DISABLE                    EXTI_Control(DISABLE)

//开启定时器
#define START_TIMER                     TIM_Cmd(TIM2, ENABLE)       //开启定时器
//关闭定时器
#define STOP_TIMER                      TIM_Cmd(TIM2, DISABLE)

#define START_NEW_SCAN                  start_new_scan();

#define EXTI_CONFIG_Rising_Falling      EXTI_Config(EXTI_Trigger_Rising_Falling)
#define EXTI_CONFIG_Rising              EXTI_Config(EXTI_Trigger_Rising)
#define EXTI_CONFIG_Falling             EXTI_Config(EXTI_Trigger_Falling)


extern void start_new_scan(void);

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#endif
