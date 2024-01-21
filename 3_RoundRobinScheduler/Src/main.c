#include <stdio.h>
#include "led.h"
#include "uart.h"
#include "timebase.h"


int main(void)
{



	led_init();
	uart_tx_init();
	timebase_init();




	while(1){

		printf("hello from stm32f411 uart + timebase \n\r");
		led_on();
		delay(1);
		led_off();
		delay(1);

	}

}
