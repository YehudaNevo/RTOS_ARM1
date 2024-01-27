#include "osKernel.h"
#include "uart.h"


void task1(void) {
    while (1) {
        printf("Task 1 is running\n");
        osThreadYield();
    }
}

void task2(void) {
    while (1) {
        printf("Task 2 is running\n");
        osThreadYield();
    }
}



int main(void) {

	uart_tx_init();

    osKernelInit(); // Initialize the kernel

    // Create task functions array
    void(*tasks[])(void) = {task1, task2};

    int priorities[] = {1, 1};

    osKernelAddThreads(tasks, priorities, 2);

    osKernelLaunch(10);


    while(1) {


    }

    return 0;
}
