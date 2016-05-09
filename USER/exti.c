#include "exti.h"

/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置嵌套向量中断控制器NVIC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置P[A|B|C|D|E]1为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void PB1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* config the NVIC(PB1) */
  NVIC_Configuration();
  
  /* config the extiline(PB1) clock and AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
  
  /* EXTI line(PB1) mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
  
  /* EXTI line gpio config(PB1) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //浮空输入模式
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

extern EXTITrigger_TypeDef TriggerMode;
//注意：配置后即关闭了中断
void EXTI_PB01_Config(EXTITrigger_TypeDef TM)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* EXTI line(PB1) mode config */
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = TM;          //中断触发模式
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  TriggerMode = TM;  //更新全局变量
}

/*
 * 函数名：EXTI_PB01_control
 * 描述  ：打开或关闭中断
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void EXTI_PB01_control(FunctionalState state)
{
   if(state == ENABLE)
     EXTI->IMR|=1<<1;     //不屏蔽line1上的中断
   
   else
     EXTI->IMR&=~(1<<1);  //屏蔽line1上的中断  
}

/*
//请看EXTI_Init()内部实现，此处还是使用上面效率更高
void EXTI_PB01_control(FunctionalState state)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  
  // EXTI line(PB1) mode config  
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = TriggerMode;
  EXTI_InitStructure.EXTI_LineCmd = state;
  EXTI_Init(&EXTI_InitStructure);
}
*/

