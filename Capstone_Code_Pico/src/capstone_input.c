#include <stdio.h>
#include <string.h>

#include "capstone_input.h"
#include "capstone_morse.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#define BUTTON_GPIO 18
#define SEND_GPIO 19

#define DEBOUNCE_US 20000

char test[10];
int state = 0;

//debouncing call
absolute_time_t prev = {0};

void gpio_callback(uint gpio, uint32_t events)
{
    //If multiple interrupts occur within ~25ms, then ignore them
    absolute_time_t cur = get_absolute_time();
    if(absolute_time_diff_us(prev,cur) < DEBOUNCE_US) return;
    prev = cur;

    if(gpio==BUTTON_GPIO)
    {
        //input button pressed
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            absolute_time_t start_time = get_absolute_time();
            state = 1;
            interpret_buttons(state);
        }
        //input button released
        if(events & GPIO_IRQ_EDGE_RISE)
        {
            absolute_time_t end_time = get_absolute_time();
            state = 2;
            interpret_buttons(state);
        }
    }
    else if(gpio==SEND_GPIO)
    {
        //send button pressed
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            absolute_time_t duration = get_absolute_time();
            state = 3;
            interpret_buttons(state);
        }
    }
}
void init_interrupts()
{
    //GPIO 18 is input button, enabled to detect rising/falling interrupts for morse timing
    gpio_set_irq_enabled_with_callback(BUTTON_GPIO,GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE,true,&gpio_callback);
    //GPIO 19 is send button, only needs falling interrupt in order to handle send
    gpio_set_irq_enabled_with_callback(SEND_GPIO,GPIO_IRQ_EDGE_FALL,true,&gpio_callback);
}