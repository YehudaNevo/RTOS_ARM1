#include <stdio.h>
#include "led.h"
#include "uart.h"

int main(void)
{


//	int _write(int file, char *ptr, int len) {
//	    int i;
//	    for (i = 0; i < len; i++) {
//	        uart_write(ptr[i]);
//	    }
//	    return len;
//	}


//	__io_putchar__('A'); // Should send the character 'A' over UART



//	led_init();


	uart_tx_init();


//debuging method
//const char *str = "hello from stm32f411\n";
//
//    while (*str) {
//        uart_write(*str++);
//    }


//    setbuf(stdout, NULL);

	while(1){

		printf("hello from stm32f411 uart\n\r");
	    for (int i = 0; i < 1000000; i++) {}


	}

}
