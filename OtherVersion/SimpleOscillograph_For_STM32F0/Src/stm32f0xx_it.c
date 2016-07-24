/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include "mydef.h"
#include "gpio.h"
#include "tim.h"

extern u32 interval_time;
extern u32 FS;
extern u8 is_abort;
extern u8 scan_mode;
extern volatile u8 trigger_mode;

extern void EXTI_Config(EXTITrigger_TypeDef TM);
extern void EXTI_Control(FunctionalState state);


/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_usart1_rx;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line 0 and 1 interrupts.
*/
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel 1 interrupt.
*/
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel 2 and 3 interrupts.
*/
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
* @brief This function handles TIM1 break, update, trigger and commutation interrupts.
*/
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_BRK_UP_TRG_COM_IRQn 0 */

  /* USER CODE END TIM1_BRK_UP_TRG_COM_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_BRK_UP_TRG_COM_IRQn 1 */

  /* USER CODE END TIM1_BRK_UP_TRG_COM_IRQn 1 */
}

/* USER CODE BEGIN 1 */
extern volatile u16 value_i;
extern volatile uint16_t ADC_ConvertedValue[];
#if USE_CHANNEL_C1
extern volatile u16 C1_value[];
#endif
#if USE_CHANNEL_C2
extern volatile u16 C2_value[];
#endif
#if USE_CHANNEL_C3
extern volatile u16 C3_value[];
#endif
extern volatile u8 SCAN_IS_OK;

extern void send(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim1)
    {
#if USE_CHANNEL_C1
        C1_value[value_i] = ADC_ConvertedValue[0];  // 读取转换的AD值
#endif
#if USE_CHANNEL_C2
        C2_value[value_i] = ADC_ConvertedValue[1];  // 读取转换的AD值
#endif
#if USE_CHANNEL_C3
        C3_value[value_i] = ADC_ConvertedValue[2];  // 读取转换的AD值
#endif
        if (value_i++ == num_of_data - 1)
        {
            STOP_TIMER;
            value_i = 0;
            SCAN_IS_OK = 1;   //send
            LED(OFF);
        }
    }
}

/* I/O线中断，中断线为PB01 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    EXTI_DISABLE;                               //禁止触发，发送完成后或者abort(且scan_mode == 0)后再次打开
    
    //使用定时器精确采样，支持三通道
    START_TIMER_NEW_SCAN;
}

void start_new_scan(void)
{
    is_abort = 1;
    value_i = 0;
#if USE_CHANNEL_C1
    C1_value[0] = ADC_ConvertedValue[0];  //先采集第一组数据
#endif
#if USE_CHANNEL_C2
    C2_value[0] = ADC_ConvertedValue[1];  //先采集第一组数据
#endif
#if USE_CHANNEL_C3
    C3_value[0] = ADC_ConvertedValue[2];  //先采集第一组数据
#endif
    value_i++;

    START_TIMER;
    LED(ON);
}

/*
 *功能：串口中断接收上位机命令并处理
 *说明：
 */
void process(u8 * rx);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static u8 i, UART1_BUFF[10];
    static u8 c;  //为了速度

    c = (u8)huart->Instance->RDR;
    UART1_BUFF[i++] = c;

    //如果遇到终止符（空格（自定义的））
    if (c == ' ')
    {
        UART1_BUFF[i] = '\0';
        i = 0;

        process(UART1_BUFF);
    }
}

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
    t[2] = '\0';  //此位置原来为冒号

    num = rx[3] - 48; //冒号后面的一个值

    if (!strcmp(t, "FS"))
    {
        STOP_TIMER;
        //设置采样率
        switch (num)
        {
        case 0 : FS = 100;      TIM_ConfigFrequency(FS); break; //最小为100Hz
        case 1 : FS = 1000;     TIM_ConfigFrequency(FS); break;
        case 2 : FS = 10000;    TIM_ConfigFrequency(FS); break;
        case 3 : FS = 100000;   TIM_ConfigFrequency(FS); break;
        case 4 : FS = 250000;   TIM_ConfigFrequency(FS); break;
        case 5 : FS = 500000;   TIM_ConfigFrequency(FS); break;
        case 6 : FS = 500000;   TIM_ConfigFrequency(FS); break;
        }

        START_TIMER_NEW_SCAN;
    }
    else if (!strcmp(t, "TM"))
    {
        //触发方式
        trigger_mode = num;
        is_abort = 1;
        switch (num)
        {
        case 0 :
            //auto
            START_TIMER_NEW_SCAN;
            break;
        case 1 :
            EXTI_CONFIG_Rising;
            EXTI_ENABLE;
            break;  //Rising
        case 2 :
            EXTI_CONFIG_Falling;
            EXTI_ENABLE;
            break;  //Falling
        case 3 :
            EXTI_CONFIG_Rising_Falling;
            EXTI_ENABLE;
            break;  //Rising_Falling
        case 4 :
            EXTI_DISABLE;
            break;  //
        }
    }
    else if (!strcmp(t, "SM"))
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
            START_TIMER_NEW_SCAN;   //开启定时器采集数据
        }
    }
    else if (!strcmp(t, "AB"))
    {
        if (scan_mode == 0)
        {
            //放弃本次采样，重新开始采集
            STOP_TIMER;
            is_abort = 1;
            value_i = 0;
        }
    }
    else if (!strcmp(t, "TG"))
    {
        START_TIMER_NEW_SCAN;
    }
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
