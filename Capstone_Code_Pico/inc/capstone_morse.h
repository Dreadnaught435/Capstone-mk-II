#ifndef CAPSTONE_MORSE_H
#define CAPSTONE_MORSE_H

#include <stdint.h>

#include "pico/time.h"

extern absolute_time_t start_time;
extern absolute_time_t end_time;
extern int state;

void interpret_buttons(int st);

#endif