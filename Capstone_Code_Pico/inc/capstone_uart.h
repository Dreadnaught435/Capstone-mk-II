#ifndef CAPSTONE_UART_H
#define CAPSTONE_UART_H

#define UART_ID uart0
#define UART_TX 12
#define UART_RX 13
#define BAUD_RATE 1500000

extern char rec[10];

void initialize_uart();
void on_uart_rx();

#endif