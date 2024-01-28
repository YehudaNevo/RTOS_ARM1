#include "osKernel.h"
#include "uart.h"

void task1(void) { while (1) { printf("Task 1 is running\n"); osThreadYield(); } }
void task2(void) { while (1) { printf("Task 2 is running\n"); osThreadYield(); } }
void task3(void) { while (1) { printf("Task 3 is running\n"); osThreadYield(); } }
void task4(void) { while (1) { printf("Task 4 is running\n"); osThreadYield(); } }
void task5(void) { while (1) { printf("Task 5 is running\n"); osThreadYield(); } }
void task6(void) { while (1) { printf("Task 6 is running\n"); osThreadYield(); } }
void task7(void) { while (1) { printf("Task 7 is running\n"); osThreadYield(); } }
void task8(void) { while (1) { printf("Task 8 is running\n"); osThreadYield(); } }
void task9(void) { while (1) { printf("Task 9 is running\n"); osThreadYield(); } }
void task10(void) { while (1) { printf("Task 10 is running\n"); osThreadYield(); } }

int main(void) {
    uart_tx_init();

    osKernelInit();

    void(*tasks[])(void) = {task1, task2, task3, task4, task5, task6, task7, task8, task9, task10};

    int priorities[] = {10, 9, 8, 7, 20, 5, 4, 3, 100, 11};

    osKernelAddThreads(tasks, priorities, 10);

    osKernelLaunch(100);

    while(1) {
    }

    return 0;
}
