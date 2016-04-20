#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f10x.h"
void PB1_Config(void);
void EXTI_PB01_Config(EXTITrigger_TypeDef TM);
void EXTI_PB01_control(FunctionalState state);

#endif /* __XXX_H */
