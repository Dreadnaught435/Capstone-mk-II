#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "capstone_uart.h"
#include "capstone_morse.h"

#include "pico/stdlib.h"
#include "hardware/uart.h"

char rec[10];

void initialize_uart()
{
    //set TX and RX pin functions
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);
    uart_init(UART_ID, BAUD_RATE);

    uart_set_fifo_enabled(UART_ID, true);
    // //set up rx interrupt handler
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}
void on_uart_rx()
{
    char c;
    static uint8_t rec_index = 0;
    if (rec_index == 0) memset(rec,0,sizeof(rec));
    while(uart_is_readable(UART_ID))
    {
        c = uart_getc(UART_ID);
        rec[rec_index] = c;
        rec_index++;
        if(c == '\n')
        {
            rec_index = 0;
        }
    }
}