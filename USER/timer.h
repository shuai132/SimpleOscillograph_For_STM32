#ifndef TIMER_H
#define TIMER_H

#include "stm32f10x.h"
 


void TIM_NVIC_Configuration(void);
void TIM_Configuration(u32 frequency);

#endif	/* TIMER_H */
