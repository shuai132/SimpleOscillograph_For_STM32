/********************** 移植时要特别关注的 *******************************
* 作者：刘帅
* 日期：2015/10/31
* 修改：2016/01/10
* 修改：2016/04/01  add USB
************************************************************************/
#ifndef __MYDEF_H
#define __MYDEF_H

#include <stdarg.h>

#define VCC_V 3.3      //float  取样电压最大值
#define ADC_B 12      //int     ADC位数

//是否使用虚拟串口
#define USE_VIRTUAL_COM 0

//上传数据
#define PRINTF_LABVIEW(format, ...)  printf(format, ##__VA_ARGS__)


#define num_of_data 1024

//触发中断允许
#define EXTI_ENABLE    EXTI_PB01_control(ENABLE)
//触发中断禁止
#define EXTI_DISABLE  EXTI_PB01_control(DISABLE)

//开启定时器
#define START_TIMER            TIM_Cmd(TIM2, ENABLE)     //开启定时器
//关闭定时器
#define STOP_TIMER             TIM_Cmd(TIM2, DISABLE)

#define START_TIMER_NEW_SCAN   start_new_scan();

#define EXTI_CONFIG_Rising_Falling    EXTI_PB01_Config(EXTI_Trigger_Rising_Falling)
#define EXTI_CONFIG_Rising            EXTI_PB01_Config(EXTI_Trigger_Rising)
#define EXTI_CONFIG_Falling           EXTI_PB01_Config(EXTI_Trigger_Falling)


extern void start_new_scan(void);

#endif
