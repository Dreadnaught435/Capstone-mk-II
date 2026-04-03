#include <stdio.h>
#include <string.h>

#include "capstone_input.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#define BUTTON_GPIO 18
#define SEND_GPIO 19

char test[10];

void gpio_callback(uint gpio, uint32_t events)
{
    if(gpio==BUTTON_GPIO)
    {
        test[1] = 'b';
    }
    else
    {
        test[2] = 'c';
    }
}
void init_interrupts()
{
    //GPIO 18 is input button, enabled to detect rising/falling interrupts for morse timing
    gpio_set_irq_enabled_with_callback(BUTTON_GPIO,GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE,true,&gpio_callback);
    //GPIO 19 is send button, only needs falling interrupt in order to handle send
    gpio_set_irq_enabled_with_callback(SEND_GPIO,GPIO_IRQ_EDGE_FALL,true,&gpio_callback);
}