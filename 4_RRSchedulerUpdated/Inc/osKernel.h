#ifndef __OSKERNEL_H__
#define __OSKERNEL_H__
#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx.h"

#define SR_UIF (1U << 0)



extern volatile uint32_t systick_flag;

void osKernelInit();
uint8_t osKernelAddThreads(void(*tasks[])(void), int priorities[], uint8_t numTasks);
void osKernelLaunch(uint32_t quanta);
void osSchedulerLaunch(void);
void osKernelStackInit(int i);
void osThreadYield(void);
void time2_1hz_interrupt_init(void);





#endif
