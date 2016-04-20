/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include "stm32f10x_it.h"
#include "mydef.h"	//num_of_data
#include "Time.h"
#include "led.h"
#include "usart1.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern u32 interval_time;
extern u32 FS;
extern u8 is_abort;
extern u8 scan_mode;
extern volatile u8 trigger_mode;

/* Private function prototypes -----------------------------------------------*/
extern void EXTI_PB01_Config(EXTITrigger_TypeDef TM);
extern void EXTI_PB01_control(FunctionalState state);
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    //TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval : None
  */
extern volatile u16 value_i;
extern __IO uint16_t ADC_ConvertedValue[3];
extern __IO u16 C1_value[num_of_data];
extern __IO u16 C2_value[num_of_data];
extern __IO u16 C3_value[num_of_data];
extern void send(void);
extern volatile u8 SCAN_IS_OK;

void TIM2_IRQHandler(void)
{
    if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
    {
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
        
        C1_value[value_i] = ADC_ConvertedValue[0];	// 读取转换的AD值
        C2_value[value_i] = ADC_ConvertedValue[1];	// 读取转换的AD值
        C3_value[value_i] = ADC_ConvertedValue[2];	// 读取转换的AD值
        
        if(value_i++ == num_of_data-1)
        {
            STOP_TIMER;
            value_i = 0;
            SCAN_IS_OK = 1;		//send
					  LED(OFF);
        }
    }
}

/* I/O线中断，中断线为PB01 */
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET) //确保是否产生了EXTI Line中断
    {
        EXTI_DISABLE;														//禁止触发，发送完成后或者abort(且scan_mode == 0)后再次打开
        EXTI_ClearITPendingBit(EXTI_Line1);     //清除中断标志位 (注意位置）
        
        //使用定时器精确采样，支持三通道
        START_TIMER_NEW_SCAN;
    }
}

void start_new_scan(void)
{
	is_abort = 1;
	value_i = 0;
	C1_value[0] = ADC_ConvertedValue[0];	// 先采集第一组数据
  C2_value[0] = ADC_ConvertedValue[1];	// 先采集第一组数据
  C3_value[0] = ADC_ConvertedValue[2];	// 先采集第一组数据
  value_i++;
	
	START_TIMER;
	LED(ON);
}

/*
 *功能：串口中断接收上位机命令并处理
 *说明：
 */
void process(u8 * rx);
void USART1_IRQHandler(void)
{
    static u8 i, UART1_BUFF[10];
    static u8 c;	//为了速度
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    { 	
        c = USART1->DR;
        UART1_BUFF[i++] = c;
			
				//如果遇到终止符（空格（自定义的））
        if(c == ' ')
        {
            UART1_BUFF[i] = '\0';
            i = 0;
            
            process(UART1_BUFF);
        }
    }
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

/*******************一些辅助函数——————by 刘帅******************************/
/*
 *功能：利用串口接收调节变量数值
 *说明：
 */
void process(u8 * rx)
{
    u8 num;
    //int nunber = 0;
    char t[3];
    t[0] = rx[0];
    t[1] = rx[1];
    t[2] = '\0';	//此位置原来为冒号
    
    num = rx[3] - 48;	//冒号后面的一个值
    
    if(!strcmp(t, "FS"))
    {
        STOP_TIMER;
        //设置采样率
        switch (num)
        {
        case 0 : FS = 100;  	TIM2_Configuration(FS);break;//最小为100Hz
        case 1 : FS = 1000; 	TIM2_Configuration(FS);break;
        case 2 : FS = 10000;  TIM2_Configuration(FS);break;
        case 3 : FS = 100000; TIM2_Configuration(FS);break;
        case 4 : FS = 250000; TIM2_Configuration(FS);break;
				case 5 : FS = 500000; TIM2_Configuration(FS);break;
        case 6 : FS = 500000; TIM2_Configuration(FS);break;
        }
				
				START_TIMER_NEW_SCAN	
    }
    else if(!strcmp(t, "TM"))
    {
			  //触发方式
			  trigger_mode = num;
        
        switch (num)
        {
					case 0 : START_TIMER_NEW_SCAN;        break;
					case 1 : EXTI_CONFIG_Rising;					break;	//up
					case 2 : EXTI_CONFIG_Falling; 				break;	//dowm
					case 3 : EXTI_CONFIG_Rising_Falling; 	break;	//auto
        }
				if(scan_mode == 0)
				{
					//EXTI_ENABLE;
					is_abort = 1;
				}
    }
    else if(!strcmp(t, "SM"))
    {
        //scan_mode
        scan_mode = num;
        if (scan_mode == 1)
        {
            //实时采集显示模式
            EXTI_DISABLE;
            is_abort = 1;
            value_i = 0;
					  PRINTF_LABVIEW("FS:0\r\n");
        }
        else
        {
						//采集一次(发送完成会自动打开中断)
						START_TIMER_NEW_SCAN;		//开启定时器采集数据
        }
    }
    else if(!strcmp(t, "AB"))
    {
			  if (scan_mode == 0)
				{
					//放弃本次采样，重新开始采集
					STOP_TIMER;
					is_abort = 1;
          value_i = 0;
				}
        
    }
}
