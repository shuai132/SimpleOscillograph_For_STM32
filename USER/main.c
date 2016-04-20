/************************SimpleOscillograph For STM32******************************
 * 文件名  ：main.c
 * 描述    ：SimpleOscillograph For STM32      
 * 平台    ：STM32核心板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：刘帅
 * 日期    ：2015-10-24
 * 修改    ：2016-01-26
 * 修改    ：2016/01/10
 * 修改    ：2016/04/01  增加USB
 * 修改    ：2016/04/14  增加自动触发改为一直触发 增加边沿触发
 * 注意    ：中断优先级：串口0-0.0 > 定时器2-1.0 > 外部中断PB01-2.0
**********************************************************************************/

#include <string.h>
#include "stm32f10x.h"
#include "usart1.h"
#include "adc.h"
#include "exti.h"
#include "led.h"
#include "Time.h"
#include "mydef.h"
#include "HW_Config.h"
#include "usb_mem.h"

/* Private function prototypes -----------------------------------------------*/
void scan(void);
void send(void);
void on_send_finish(void);


// interval_time 采样时间间隔：1/FS
volatile u32 FS = 100000u;				//初始显示100KHz
//channel
volatile u8 is_one_channel = 0;
//abort flag
volatile u8 is_abort = 0;         //1：放弃本次发送
// scan mode
volatile u8 scan_mode 	= 0;		  //0：实时采样（正常模式） 1：采集的同时发送数据 适合低频信号
volatile u16 value_i		= 0;	  	//记录正在采集到本次第几个点  范围：0至(number_of_data-1)
volatile u8 SCAN_IS_OK	= 0;		  //标记是否为采集完毕
// TriggerMode
volatile u8 trigger_mode 	= 0;		  //0：发送完成自动触发 1 2 3分别为上升 下降 边沿触发
volatile EXTITrigger_TypeDef TriggerMode;  //用于记录中断模式(触发模式)

// 缓冲区 从触发开始，以采样频率FS存储num_of_data个点，之后一并发给串口
__IO u16 C1_value[num_of_data];
__IO u16 C2_value[num_of_data];
__IO u16 C3_value[num_of_data];

// ADC转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[3];

/*
 * 主函数
 */
int main(void)
{
	// 局部变量，用于保存转换计算后的电压值			 
	u16 ADC_ConvertedValueLocal;
	
	#if USE_VIRTUAL_COM
	  //初始化USB串行设备（虚拟串口）
	  USB_Config();
	#endif
	
	/* LED CONFIG */
	//PC13
	LED_GPIO_Config();
	LED(ON);
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* TIM2 定时配置 */
	TIM2_NVIC_Configuration();
  TIM2_Configuration(FS);	//初始FS(100KHz)
	
	/* USART1 config */
	USART1_Config();
	
	/* enable adc1 and config adc1 to dma mode */
	ADC1_Init();
	
	/* exti line config */
	PB1_Config();
	
	//初始化关闭触发中断
	EXTI_DISABLE;
	
	PRINTF_LABVIEW("\r\n------ SimpleOscillograph For STM32 ------\r\n");
	
	//发送ADC信息，供上位机处理ad原始值
	PRINTF_LABVIEW("VC:%f\r\n", VCC_V);	//VCC
	PRINTF_LABVIEW("AD:%d\r\n", ADC_B);	//adc位数
	
	//开始一次采集，用于调试
	START_TIMER_NEW_SCAN;
	while (1)
	{
		if (scan_mode == 1)
		{
			//实时采样实时显示
			ADC_ConvertedValueLocal = ADC_ConvertedValue[0]; // 读取转换的AD值
			PRINTF_LABVIEW("C1:%d\r\n", ADC_ConvertedValueLocal); 
		
			ADC_ConvertedValueLocal = ADC_ConvertedValue[1]; // 读取转换的AD值
			PRINTF_LABVIEW("C2:%d\r\n", ADC_ConvertedValueLocal); 
		
			ADC_ConvertedValueLocal = ADC_ConvertedValue[2]; // 读取转换的AD值
			PRINTF_LABVIEW("C3:%d\r\n", ADC_ConvertedValueLocal);
		}
		if(SCAN_IS_OK == 1)
		{
			SCAN_IS_OK = 0;
			send();
		}
	}
	//return 0;
}

/*
 * 数据发送函数，注意开头和结尾要有一定冗余（填充上位机的缓冲区）
 * 直接发送原始值，发至上位机处理
 * 发送完毕后，启用外部中断以允许再次触发
 */
void send(void)
{
	u16 i = 0;
	
	//发送ADC信息，供上位机处理ad原始值
	PRINTF_LABVIEW("VC:%f\r\n", VCC_V);	//VCC
	PRINTF_LABVIEW("AD:%d\r\n", ADC_B);	//adc位数
	PRINTF_LABVIEW("FS:%d\r\n", FS);
	PRINTF_LABVIEW("NM:%d\r\n", num_of_data);
	
	is_abort = 0;

	do
	{
		PRINTF_LABVIEW("C1:%d\r\n", C1_value[i]);
		PRINTF_LABVIEW("C2:%d\r\n", C2_value[i]);
		PRINTF_LABVIEW("C3:%d\r\n", C3_value[i]);
	}
	while((++i < num_of_data) && (is_abort == 0) && (scan_mode == 0));
	
	PRINTF_LABVIEW("FN:OK\r\n");
	PRINTF_LABVIEW("SEND_FINISHED!!!\r\n");	//冗余内容
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
			//发送完成自动触发
			//开始一次采集，用于调试
	    START_TIMER_NEW_SCAN;
		}
		else
		{
	    EXTI_ENABLE;	//send finish enable_exti
		}
	}
}


/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口 */
	
	#if USE_VIRTUAL_COM
	  static unsigned char tmp[1];
	  tmp[0] = ch;
	  USB_TxWrite(tmp, 1);
	#else
	  USART_SendData(USART1, (unsigned char) ch);
	  while (!(USART1->SR & USART_FLAG_TXE));
	#endif
	
	return (ch);
}

