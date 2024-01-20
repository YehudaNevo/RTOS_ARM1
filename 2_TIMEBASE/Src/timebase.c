#include "timebase.h"
#include "stm32f4xx.h"

// Constants for SysTick Timer configuration
#define ONE_SEC_LOAD 16000000U    // Load value for 1 second; system clock is 16 MHz
#define CTRL_ENABLE (1U << 0)     // Enable SysTick Timer
#define CTRL_TICKINT (1U << 1)    // Enable SysTick interrupt
#define CTRL_CLCKSRC (1U << 2)    // SysTick clock source: processor clock
#define CTRL_COUNTFLAG (1U << 16) // Timer count-to-0 flag (read-only)
#define MAX_DELAY 0xFFFFFFFFU     // Maximum delay value

// Global variables for tracking time
volatile uint32_t g_curr_tick;
volatile uint32_t g_curr_tick_p;
volatile uint32_t tick_freq = 1;


void delay(uint32_t delay)
{
    uint32_t tickstart = get_tick();
    uint32_t wait = delay;

    // Adjust delay to ensure accuracy
    if (wait < MAX_DELAY)
    {
        wait += tick_freq;
    }

    while ((get_tick() - tickstart) < wait) { }
}


void tick_increment(void)
{
    g_curr_tick += tick_freq;
}


void timebase_init(void)
{
    // Set the timer reload value for a 1-second interval
    SysTick->LOAD = ONE_SEC_LOAD - 1;

    // Reset the current timer value
    SysTick->VAL = 0;

    // Configure and enable the SysTick timer
    SysTick->CTRL = CTRL_CLCKSRC | CTRL_TICKINT | CTRL_ENABLE;

    // Enable global interrupts for interrupt handling
    __enable_irq();
}


uint32_t get_tick(void)
{
    __disable_irq();           // Disable interrupts to read value safely
    g_curr_tick_p = g_curr_tick;
    __enable_irq();            // Re-enable interrupts

    return g_curr_tick_p;
}


//SysTick interrupt handler. Called every tick.

void SysTick_Handler(void)
{
    tick_increment();
}
