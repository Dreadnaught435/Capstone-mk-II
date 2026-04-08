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

int state = 0;

//led for debugging
#include "pico/stdlib.h"
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}
// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

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

    if(gpio==BUTTON_GPIO)
    {
        //input button pressed
        if(events & GPIO_IRQ_EDGE_FALL)
        {
            // int rc = pico_led_init();
            // hard_assert(rc == PICO_OK);
            // pico_set_led(true);
            // sleep_ms(LED_DELAY_MS);
            // pico_set_led(false);
            //get time of button press
            start_time = get_absolute_time();
            state = 1;
            //interpret what the duration of the last unpress means
            interpret_buttons(state);
        }
        //input button released
        if(events & GPIO_IRQ_EDGE_RISE)
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
}
void init_interrupts()
{
    //GPIO 18 is input button, enabled to detect rising/falling interrupts for morse timing
    gpio_set_irq_enabled_with_callback(BUTTON_GPIO,GPIO_IRQ_EDGE_FALL|GPIO_IRQ_EDGE_RISE,true,&gpio_callback);
    //GPIO 19 is send button, only needs falling interrupt in order to handle send
    gpio_set_irq_enabled_with_callback(SEND_GPIO,GPIO_IRQ_EDGE_FALL,true,&gpio_callback);
}