#include <stdio.h>
#include "led.h"
#include "uart.h"
#include "timebase.h"

// Function prototypes
void motor_on(void);
void motor_off(void);
void valve_open(void);
void valve_close(void);

int main(void)
{
    led_init();
    uart_tx_init();
    timebase_init();

    while(1)
    {
        printf("Hello from STM32F411: UART + Timebase\n\r");

        led_on();
        delay(1);
        led_off();
        delay(1);

        motor_on();
        delay(1);
        motor_off();
        delay(1);

        valve_open();
        delay(1);
        valve_close();
        delay(1);
    }
}

// Function to turn the motor on
void motor_on(void)
{
    printf("Motor ON\n\r");
    // Add motor control code here
}

// Function to turn the motor off
void motor_off(void)
{
    printf("Motor OFF\n\r");
    // Add motor control code here
}

// Function to open the valve
void valve_open(void)
{
    printf("Valve OPEN\n\r");
    // Add valve control code here
}

// Function to close the valve
void valve_close(void)
{
    printf("Valve CLOSE\n\r");
    // Add valve control code here
}
