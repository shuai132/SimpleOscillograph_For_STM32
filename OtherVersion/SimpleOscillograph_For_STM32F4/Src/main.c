/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "mydef.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void scan(void);
void send(void);
void on_send_finish(void);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

//interval_time 采样时间间隔：1/FS
volatile u32 FS         = 100000u;  //初始显示100KHz
//abort flag
volatile u8 is_abort    = 0;        //1：放弃本次发送
// scan mode
volatile u8 scan_mode   = 0;        //0：实时采样（正常模式） 1：采集的同时发送数据 适合低频信号
volatile u16 value_i    = 0;        //记录正在采集到本次第几个点  范围：0至(number_of_data-1)
volatile u8 SCAN_IS_OK  = 0;        //标记是否为采集完毕
// TriggerMode
volatile u8 trigger_mode= 0;        //0：发送完成自动触发 1 2 3分别为上升 下降 边沿触发
volatile EXTITrigger_TypeDef TriggerMode;  //用于记录中断模式(触发模式)


// 缓冲区 从触发开始，以采样频率FS存储num_of_data个点，之后一并发给串口
#if USE_CHANNEL_C1
volatile u16 C1_value[num_of_data];
#endif
#if USE_CHANNEL_C2
volatile u16 C2_value[num_of_data];
#endif
#if USE_CHANNEL_C3
volatile u16 C3_value[num_of_data];
#endif

// ADC转换的电压值通过MDA方式传到SRAM
volatile uint16_t ADC_ConvertedValue[3];

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  u8 no_use;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
    SystemCoreClockUpdate();
    printf("SystemCoreClock = %d\r\n", SystemCoreClock);
    
    /* enable adc1 and config adc1 to dma mode */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_ConvertedValue, 3);
    
    /* 关闭DMA中断 否则会非常非常浪费资源 CubeMX没有禁用的选项所以为了移植性在这里关闭 */
    HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
    
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)&no_use, 1);
    
    /* LED */
    LED(ON);

    /* TIM 定时配置 */
    TIM_ConfigFrequency(FS);  //初始FS(100KHz)
    
    //初始化关闭触发中断
    EXTI_DISABLE;

    PRINTF_LABVIEW("\r\n------ SimpleOscillograph For STM32 ------\r\n");

    //发送ADC信息，供上位机处理ad原始值
    PRINTF_LABVIEW("VC:%f\r\n", VCC_V);  //VCC
    PRINTF_LABVIEW("AD:%d\r\n", ADC_B);  //adc位数

    //开始一次采集，用于调试
    START_NEW_SCAN;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
        if (scan_mode == 1)
        {
            //实时采样实时显示
            PRINTF_LABVIEW("C1:%d\r\nC2:%d\r\nC3:%d\r\n", ADC_ConvertedValue[0], ADC_ConvertedValue[1], ADC_ConvertedValue[2]);
        }
        if(SCAN_IS_OK == 1)
        {
            SCAN_IS_OK = 0;
            send();
        }
    }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 3, 0);
}

/* USER CODE BEGIN 4 */
/*
 * 数据发送函数
 * 直接发送原始值，发至上位机处理
 * 发送完毕后，启用外部中断以允许再次触发
 */
void send(void)
{
    u16 i = 0;

    //发送ADC信息，供上位机处理ad原始值
    PRINTF_LABVIEW("VC:%f\r\n", VCC_V);       //VCC
    PRINTF_LABVIEW("AD:%d\r\n", ADC_B);       //adc位数
    PRINTF_LABVIEW("FS:%d\r\n", FS);          //采样率
    PRINTF_LABVIEW("NM:%d\r\n", num_of_data); //数据个数(用于绘图）

    is_abort = 0;

    do
    {
#if USE_CHANNEL_C1
        PRINTF_LABVIEW("C1:%d\r\n", C1_value[i]);
#endif

#if USE_CHANNEL_C2
        PRINTF_LABVIEW("C2:%d\r\n", C2_value[i]);
#endif

#if USE_CHANNEL_C3
        PRINTF_LABVIEW("C3:%d\r\n", C3_value[i]);
#endif
    }
    while((++i < num_of_data) && (is_abort == 0) && (scan_mode == 0));

    PRINTF_LABVIEW("FN:OK\r\n");    //也利用这次发送为ADC启动转换延时到稳定
    on_send_finish();
}


//发送完毕需要重置一些参数和设置
void on_send_finish(void)
{
    LED(OFF);
    if(scan_mode == 0)
    {
        if(trigger_mode == 0)
        {
            //自动开始新的采集
            START_NEW_SCAN;
        }
        else if(trigger_mode == 4)
        {
            EXTI_DISABLE;
        }
        else
        {
            EXTI_ENABLE;
        }
    }
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
