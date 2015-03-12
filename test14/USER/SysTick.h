#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void SysTick_Init(void);
void delay_ms(__IO u16 nms);
void delay_us(__IO u32 nTime);

#endif /* __SYSTICK_H */
