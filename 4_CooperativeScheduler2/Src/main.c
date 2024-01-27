#include "osKernel.h"
#include "uart.h"
//#include "timebase.h"

#define QUANTA_TIME 10

// Task function prototypes
void task1(void);
void task2(void);

volatile uint32_t systick_flag = 0;


int main(void) {

    uart_tx_init();


    // Create an array of task functions
    void (*tasks[])(void) = {&task1, &task2};

    // Add tasks to the OS Kernel
    osKernelAddThreads(tasks, 2);

    // Launch the OS scheduler
    osKernelLaunch(QUANTA_TIME);



    while(1) {

    	if (systick_flag ) {
    	   printf("SysTick Interrupt occurred\n");
    	    systick_flag = 0;
    	}
    }
}



void task1(void) {
    while(1) {
        printf("Task 1 is running\n");
       // delay(1);
    }
}

void task2(void) {
    while(1) {
        printf("Task 2 is running\n");
        //delay(1);
    }
}
