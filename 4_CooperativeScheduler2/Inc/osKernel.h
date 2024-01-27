#ifndef __OSKERNEL_H__
#define __OSKERNEL_H__
#include <stdint.h>
#include "stm32f4xx.h"


extern volatile uint32_t systick_flag;

void osKernelInit();
uint8_t osKernelAddThreads(void(*tasks[])(void), uint8_t numTasks);
void osKernelLaunch(uint32_t quanta);
void osSchedulerLaunch(void);
void osKernelStackInit(int i);




#endif
