#include <stdint.h>
#include "uart.h"
#include "stm32f4xx.h"

#define GPIOAEN (1U<<0)
#define UART2EN  (1U<<17)

#define SYS_FREQ 16000000
#define APB1_CLK SYS_FREQ
#define UART_BOUDRATE 115200

#define CR1_TE  (1U<<3)
#define CR1_UE  (1U <<13)

#define SR_TXE (1U <<7)

static uint16_t compute_uart_pd(uint32_t perph_clk , uint32_t boudrate);
static void uart_set_boudrate(uint32_t perph_clk , uint32_t boudrate);
void uart_write(int ch);

int __io_putchar(int ch){

	uart_write(ch);
	return ch;

}

void uart_tx_init(void){

	//STEP 1

	//Enable clock access to GPOIA
	RCC->AHB1ENR |= GPIOAEN;

	// set PA2 mode to alternate function mode
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);
	// set alternate function type to AF07 UART2_TX
	//AFRL == AFR[0]
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~ (1U<<11);

	// STEP 2

	//enable clock access to uart2
	RCC->APB1ENR |= UART2EN;

	//configure bouderate
	uart_set_boudrate(APB1_CLK, UART_BOUDRATE);

	//configure transfer direction
	USART2->CR1 = CR1_TE;

	//enable uart module
	USART2->CR1 |= (CR1_UE);



}


void uart_write(int ch)
{
	// make sure the transmite data reg is empty
	while(!(USART2->SR & SR_TXE))
		{
		// just wait
		}
	// write data to trans reg
	USART2->DR = (ch & 0xFF);


}




static void uart_set_boudrate(uint32_t perph_clk , uint32_t boudrate)
{
	USART2->BRR = compute_uart_pd( perph_clk ,  boudrate);
}



static uint16_t compute_uart_pd(uint32_t perph_clk , uint32_t boudrate)
{
	return ((perph_clk + (boudrate/2U)) / boudrate);

}



