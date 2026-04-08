#ifndef CAPSTONE_INPUT_H
#define CAPSTONE_INPUT_H

#include <stdint.h>
#include "pico/stdlib.h"
 
void init_interrupts();
void gpio_callback(uint gpio, uint32_t events);

#endif