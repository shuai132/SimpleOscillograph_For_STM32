/*
 *产生25Hz正弦波 PA4 PA5
 *请自行确保一些参数结果为整数
 */

#include <math.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dac.h"
#include "mydef.h"

uint16_t sine_table1[360];
uint16_t sine_table2[360];

void TIM2_NVIC_Configuration(void);
void TIM2_Configuration(u32 frequency);

int main(void)
{
    uint16_t i;

    NVIC_Configuration();
    delay_init();	    	//延时函数初始化
    uart_init(9600);	 	//串口初始化为9600
    Dac1_Init();		  	//DAC通道1初始化

    TIM2_NVIC_Configuration();
    TIM2_Configuration(25*360/STEP);

    //生成数据
    for(i=0; i<360; i++)
    {
        sine_table1[i] = (uint16_t)((sin((i*PI)/180) + 1)*4095/2);
        sine_table2[i] = (uint16_t)((sin((i*PI)/180 + PHA_DIFF) + 1)*4095/2);
    }
    while(1)
    {
    }
}

void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* TIM_Prescaler--71 --> 1us ++ */
void TIM2_Configuration(u32 frequency)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=(uint16_t)(1000000ul/frequency - 1);	/* 自动重装载寄存器周期的值(计数值) */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				    /* 时钟预分频数 72M/72 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式 */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
}

