#include "osKernel.h"

#define MAX_NUM_OF_THREADS 2
#define STACK_SIZE 100
#define BUS_FREQ 16000000      // Bus frequency in Hz
#define MIL_SEC_PRE_SCALE (int)(BUS_FREQ / 1000)

// SysTick Control and Status Register bits
#define CTRL_ENABLE (1U << 0)     // Enable SysTick Timer
#define CTRL_TICKINT (1U << 1)    // Enable SysTick interrupt
#define CTRL_CLCKSRC (1U << 2)    // SysTick clock source: processor clock
#define CTRL_COUNTFLAG (1U << 16) // Timer count-to-0 flag (read-only)
#define SYSTICK_RST 0             // Reset value for SysTick


// Definition for manipulating the SysTick interrupt in the NVIC
#define INTCTRL (*((volatile uint32_t *)0xE000ED04)) // Register for controlling SysTick interrupt
#define PENDSTSET (1U << 26)      // Bit to set SysTick interrupt pending

typedef struct tcb {
    int32_t *stackPtr;    // Pointer to the stack
    struct tcb *nextPtr;  // Pointer to the next thread control block
    //int priority;         // Task priority
} tcb;




tcb tcbs[MAX_NUM_OF_THREADS];
tcb *currentPtr;
int32_t tcbs_stack[MAX_NUM_OF_THREADS][STACK_SIZE];


// Function to initialize the stack of a thread
void osKernelStackInit(int i) {
    tcbs[i].stackPtr = &tcbs_stack[i][STACK_SIZE - 16]; // Top of the stack (after registers)

    // Set Thumb mode
    tcbs_stack[i][STACK_SIZE - 1] = (1U << 24);

    // Initialize R0-R12, LR for debugging
    for (int j = STACK_SIZE - 3; j >= STACK_SIZE - 16; j--) {
        tcbs_stack[i][j] = 0xAAAAAAAA;
    }
}

// Function to add threads to the kernel
uint8_t osKernelAddThreads(void(*tasks[])(void), uint8_t numTasks) {
    if (numTasks > MAX_NUM_OF_THREADS) {
        return 0; // Error if numTasks exceeds maximum
    }

    __disable_irq(); // Disable global interrupt

    for (int i = 0; i < numTasks; i++) {
        tcbs[i].nextPtr = &tcbs[(i + 1) % numTasks];
        osKernelStackInit(i);
        tcbs_stack[i][STACK_SIZE - 2] = (int32_t)tasks[i]; // Set PC to task function address
    }

    currentPtr = &tcbs[0]; // Start with the first task

    __enable_irq(); // Re-enable global interrupt

    return 1; // Success
}

// Function to initialize the kernel
void osKernelInit(void) {
    // Initialization code here (if any)
}


void osKernelLaunch(uint32_t quanta) {
    SysTick->CTRL = SYSTICK_RST;         // Reset SysTick
    SysTick->VAL = 0;                    // Clear SysTick current value register
    SysTick->LOAD = (quanta * MIL_SEC_PRE_SCALE) - 1; // Load quanta

    NVIC_SetPriority(SysTick_IRQn, 15);  // Set SysTick to lowest priority

    SysTick->CTRL = CTRL_CLCKSRC | CTRL_ENABLE | CTRL_TICKINT; // Configure and enable SysTick

    osSchedulerLaunch(); // Launch the scheduler
}




__attribute__((naked)) void SysTick_Handler(void) {

    // Save the context of the current task
    __asm volatile(
        "CPSID I\n"               // Disable interrupts
        "MRS R0, PSP\n"           // Get current PSP value
        "STMDB R0!, {R4-R11}\n"   // Save R4-R11 on the current task's stack
        "LDR R1, =currentPtr\n"   // Load address of the current TCB
        "LDR R1, [R1]\n"          // Load current TCB
        "STR R0, [R1]\n"          // Update current TCB's stack pointer
    );

    // Switch to the next task
    __asm volatile(
        "LDR R0, =currentPtr\n"   // Load address of the current TCB
        "LDR R1, [R0]\n"          // Load current TCB
        "LDR R2, [R1, #4]\n"      // Load address of the next TCB
        "STR R2, [R0]\n"          // Update current TCB to next TCB
    );

    // Restore the context of the next task
    __asm volatile(
        "LDR R0, [R2]\n"          // Load next task's stack pointer
        "LDMIA R0!, {R4-R11}\n"   // Restore R4-R11 from the next task's stack
        "MSR PSP, R0\n"           // Update PSP to the next task's stack pointer
        "CPSIE I\n"               // Enable interrupts
        "BX LR\n"                 // Return from interrupt
    );
}






void osSchedulerLaunch(void) {

    currentPtr = &tcbs[0]; // Set to the first task
    __set_PSP((uint32_t)currentPtr->stackPtr); // Set PSP to the task's stack pointer
    __set_CONTROL(__get_CONTROL() | 0x02);     // Switch to use the PSP
    __ISB();                                   // Ensure effects of previous instructions are observed

    // Manually pop the first task's initial registers
    // Assuming the registers were saved in the order: R4-R11, R0-R3, R12, LR, PC
    __asm volatile (
        "POP {R4-R11}           \n"  // Pop callee-saved registers
        "POP {R0-R3}            \n"  // Pop general-purpose registers
        "POP {R12}              \n"  // Pop R12
        "POP {LR}               \n"  // Pop return address into LR
        "POP {PC}               \n"  // Pop PC to start the task
        // Note: PC pop will cause the processor to jump to the task function
    );
}



/**
 * Triggers a SysTick interrupt to yield the current thread and switch to the next one.
 */
void osThreadYield(void)
{
    SysTick->VAL = 0;           // Reset the SysTick current value
    INTCTRL |= PENDSTSET;       // Set the SysTick interrupt pending bit to trigger an interrupt
}








