#include "led.h"

void LED_GPIO_Config(void)
{
  //定义结构体
  GPIO_InitTypeDef GPIO_InitStruct;
  //开启时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  //选择引脚
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
  //推挽输出
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
  //设置引脚速率
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  //初始化gpioc
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  //关闭led
  GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}
