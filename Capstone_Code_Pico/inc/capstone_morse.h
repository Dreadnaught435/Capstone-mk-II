#ifndef CAPSTONE_MORSE_H
#define CAPSTONE_MORSE_H

#include <stdint.h>

#include "pico/time.h"

extern volatile absolute_time_t start_time;
extern volatile absolute_time_t end_time;
extern int state;

extern char message[10];
extern char letter[4];

void interpret_buttons(int st);

#endif