/******************** (C) COPYRIGHT 2012 WildFire Team ***************************
 * 文件名  ：Time.c
 * 描述    ：TIM2 定时应用函数库
 *          
 * 实验平台：野火STM32开发板
 * 硬件连接：----------------------
 *          |					  |
 *          |	        无     	  |
 *          |					  |
 *           ----------------------
 * 库版本  ：ST3.5.0
**********************************************************************************/
#include "Time.h"


/*
 * 函数名：TIM2_NVIC_Configuration
 * 描述  ：TIM2中断优先级配置
 * 输入  ：无
 * 输出  ：无	
 */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*TIM_Period--1000   TIM_Prescaler--71 -->中断周期为1ms*/
void TIM2_Configuration(u32 frequency)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	
    TIM_TimeBaseStructure.TIM_Period=(uint16_t)(1000000ul/frequency - 1);	/* 自动重装载寄存器周期的值(计数值) */
    /* 累计TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				    /* 时钟预分频数 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式 */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, DISABLE);																		/* 关闭时钟 */
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
