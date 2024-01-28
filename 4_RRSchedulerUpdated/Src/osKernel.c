#include "osKernel.h"

#define MAX_NUM_OF_THREADS 15
#define STACK_SIZE 100
#define BUS_FREQ 16000000
#define MIL_SEC_PRE_SCALE (int)(BUS_FREQ / 1000)

// SysTick Control and Status Register bits
#define CTRL_ENABLE (1U << 0)
#define CTRL_TICKINT (1U << 1)
#define CTRL_CLCKSRC (1U << 2)
#define CTRL_COUNTFLAG (1U << 16)
#define SYSTICK_RST 0

// Definition for manipulating the SysTick interrupt in the NVIC
#define INTCTRL (*((volatile uint32_t *)0xE000ED04))
#define PENDSTSET (1U << 26)


#define TIMER2ENABLE (1U <<0)
#define CR1_CEN (1U << 0)
#define DIER_UIE (1U << 0)




typedef struct tcb {
    int32_t *stackPtr;
    struct tcb *nextPtr;
    int priority;
} tcb;



tcb tcbs[MAX_NUM_OF_THREADS];
tcb *currentPtr;
int32_t tcbs_stack[MAX_NUM_OF_THREADS][STACK_SIZE];
tcb *head = NULL;


void osKernelStackInit(int i) {
    tcbs[i].stackPtr = &tcbs_stack[i][STACK_SIZE - 16];
    tcbs_stack[i][STACK_SIZE - 1] = (1U << 24);

}

void insertTaskSorted(tcb* taskToInsert) {
    __disable_irq();

    if (head == NULL) {
        // First task in the list
        head = taskToInsert;
        taskToInsert->nextPtr = taskToInsert; // Circular list with one element
    } else {
        tcb *current = head;
        tcb *prev = NULL;

        while (1) {
            if (taskToInsert->priority > current->priority) {
                taskToInsert->nextPtr = current;

                if (prev) {
                    prev->nextPtr = taskToInsert;  // Regular insertion
                } else {
                    // Inserting a new head. First, find the last task
                    tcb *lastTask = head;
                    while (lastTask->nextPtr != head) {
                        lastTask = lastTask->nextPtr;
                    }
                    lastTask->nextPtr = taskToInsert; // Update last task to point to new head
                    head = taskToInsert;  // New task becomes the head
                }
                break;
            }

            prev = current;
            current = current->nextPtr;
            if (current == head) {
                // Inserting at the end of the list
                prev->nextPtr = taskToInsert;
                taskToInsert->nextPtr = head;
                break;
            }
        }
    }
    __enable_irq();
}

uint8_t osKernelAddThreads(void(*tasks[])(void), int priorities[], uint8_t numTasks) {
    if (numTasks > MAX_NUM_OF_THREADS) {
        return 0;
    }

    __disable_irq();

    for (int i = 0; i < numTasks; i++) {
        tcbs[i].priority = priorities[i];
        osKernelStackInit(i);
        tcbs_stack[i][STACK_SIZE - 2] = (int32_t)tasks[i];
        insertTaskSorted(&tcbs[i]);
    }

    currentPtr = head;

    __enable_irq();
    return 1;
}

void osKernelInit(void) {
    head = NULL;
}

void osKernelLaunch(uint32_t quanta) {
    SysTick->CTRL = SYSTICK_RST;
    SysTick->VAL = 0;
    SysTick->LOAD = (quanta * MIL_SEC_PRE_SCALE) - 1;
    NVIC_SetPriority(SysTick_IRQn, 15);
    SysTick->CTRL = CTRL_CLCKSRC | CTRL_ENABLE | CTRL_TICKINT;
    osSchedulerLaunch();
}

__attribute__((naked)) void SysTick_Handler(void) {

    __asm volatile(
        "CPSID I\n"
        "MRS R0, PSP\n"
        "STMDB R0!, {R4-R11}\n"
        "LDR R1, =currentPtr\n"
        "LDR R1, [R1]\n"
        "STR R0, [R1]\n"
    );

    currentPtr = currentPtr->nextPtr;

    __asm volatile(
        "LDR R0, =currentPtr\n"
        "LDR R0, [R0]\n"
        "LDR R0, [R0]\n"
        "LDMIA R0!, {R4-R11}\n"
        "MSR PSP, R0\n"
        "CPSIE I\n"
        "BX LR\n"
    );
}

void osSchedulerLaunch(void) {
    currentPtr = head;
    __set_PSP((uint32_t)currentPtr->stackPtr);
    __set_CONTROL(__get_CONTROL() | 0x02);
    __ISB();
    __asm volatile (
        "POP {R4-R11}\n"
        "POP {R0-R3}\n"
        "POP {R12}\n"
        "POP {LR}\n"
        "POP {PC}\n"
    );
}

void osThreadYield(void) {
    SysTick->VAL = 0;
    INTCTRL |= PENDSTSET;
}



void time2_1hz_interrupt_init(void)
{
    // Enable clock access to Timer 2
    RCC->APB1ENR |= TIMER2ENABLE;

    // Set Timer 2 prescaler to divide the clock by 1600
    TIM2->PSC = 1600 - 1;  // 16MHz / 1600 = 10kHz

    // Set auto-reload value for 1Hz interrupt frequency
    TIM2->ARR = 10000 - 1; // 10kHz / 10000 = 1Hz

    // Clear Timer 2 counter
    TIM2->CNT = 0;

    // Enable Timer 2
    TIM2->CR1 = CR1_CEN;

    // Enable Timer 2 update interrupt
    TIM2->DIER |= DIER_UIE;

    // Enable Timer 2 interrupt in NVIC
    NVIC_EnableIRQ(TIM2_IRQn);
}




