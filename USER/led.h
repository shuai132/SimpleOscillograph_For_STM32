#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define ON 1
#define OFF 0

#define LED(a)  if(a)\
                GPIO_SetBits(GPIOC,GPIO_Pin_13);\
                else\
                GPIO_ResetBits(GPIOC,GPIO_Pin_13);

void LED_GPIO_Config(void);
#endif
