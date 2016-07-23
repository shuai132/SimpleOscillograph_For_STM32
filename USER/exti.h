#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f10x.h"
void EXTI_PB01_Init(void);
void EXTI_Config(EXTITrigger_TypeDef TM);
void EXTI_Control(FunctionalState state);

#endif /* __XXX_H */
