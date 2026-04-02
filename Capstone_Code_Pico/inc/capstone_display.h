#ifndef CAPSTONE_DISPLAY_H
#define CAPSTONE_DISPLAY_H

#include <stdint.h>
void oled_init(void);
void oled_write_char6(const uint8_t data[6]);
void oled_send_data(uint8_t *data, uint8_t len);
void test();
void oled_send_command(uint8_t cmd);
void oled_set_cursor(uint8_t page, uint8_t column);
void i2c_init(void);
void oled_clear(void);

#endif