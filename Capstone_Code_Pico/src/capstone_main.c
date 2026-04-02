#include <stdio.h>
#include <string.h>
#include "capstone_display.h"

int main(void)
{
    start_i2c();
    oled_init();
    test();
}