#ifndef CAPSTONE_INPUT_H
#define CAPSTONE_INPUT_H

#include "pico/stdlib.h"

void init_interrupts();
void gpio_callback(uint gpio, uint32_t events);

extern char test[10];

#endif