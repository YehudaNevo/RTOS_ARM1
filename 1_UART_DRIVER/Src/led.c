#include "led.h"


#define GPIOAEN  (1U<<0) // 0b 0000 0000 0000 0000 0000 0000 0000 0001
#define LED_PIN (1U<<5)

void led_init(void)
{
	/* Enable clock access to led port(Port A) */
	RCC->AHB1ENR |= GPIOAEN;


	/* Set led pin to an output pin */

	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);



}

void led_on(void)
{
	//set led pin to HIGH PC13

	GPIOA->ODR |= LED_PIN;


}

void led_off(void)
{
	//set led pin to low
	GPIOA->ODR &= ~LED_PIN;


}








