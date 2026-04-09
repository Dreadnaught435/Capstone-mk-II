#include <stdio.h>
#include <string.h>

#include "capstone_input.h"
#include "capstone_morse.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"


#define MORSE_GPIO 18
#define SEND_GPIO 19
#define ENTER_GPIO 8
#define DELETE_GPIO 9

#define DEBOUNCE_US 20000

int state = 0;

//debouncing call
absolute_time_t prev = {0};
//duration tracking for morse decoding
volatile absolute_time_t start_time;
volatile absolute_time_t end_time;

void gpio_callback(uint gpio, uint32_t events)
{
    //If multiple interrupts occur within ~25ms, then ignore them
    absolute_time_t cur = get_absolute_time();
    if(absolute_time_diff_us(prev,cur) < DEBOUNCE_US) return;
    prev = cur;

    if(gpio==MORSE_GPIO)
    {
        //input button pressed
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            //get time of button press
            start_time = get_absolute_time();
            state = 1;
            interpret_buttons(state);
        }
        //input button unpressed
        else if(events & GPIO_IRQ_EDGE_RISE)
        {
            //get time of button unpress
            end_time = get_absolute_time();
            state = 2;
            //interpret what the duration of the last press means
            interpret_buttons(state);
        }
    }
    else if(gpio==SEND_GPIO)
    {
        //send button pressed
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            state = 3;
            interpret_buttons(state);
            //reset timestamps for next message
            start_time = 0;
            end_time = 0;
        }
    }
    else if(gpio == ENTER_GPIO)
    {
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            state = 4;
            interpret_buttons(state);
            start_time = 0;
            end_time = 0;
        }
    }
    else if(gpio == DELETE_GPIO)
    {
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            state = 5;
            interpret_buttons(state);
            start_time = 0;
            end_time = 0;
        }
    }
}
void init_interrupts()
{
    //GPIO 18 is input button
    gpio_set_irq_enabled_with_callback(MORSE_GPIO,GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE,true,&gpio_callback);
    //GPIO 19 is send button
    gpio_set_irq_enabled_with_callback(SEND_GPIO,GPIO_IRQ_EDGE_FALL,true,&gpio_callback);
    //GPIO 8 is enter button, for adding the letter to the message
    gpio_set_irq_enabled_with_callback(ENTER_GPIO,GPIO_IRQ_EDGE_FALL,true,&gpio_callback);
    //GPIO 9 is delete button, removing latest added character to letter
    gpio_set_irq_enabled_with_callback(DELETE_GPIO,GPIO_IRQ_EDGE_FALL,true,&gpio_callback);
}