#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "capstone_display.h"
#include "capstone_input.h"
#include "capstone_morse.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "raspberry26x32.h"
#include "ssd1306_font.h"

int main() {

    //inits
    stdio_init_all();
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000); //i2c
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);
    SSD1306_init(); //display
    gpio_pull_up(18);
    gpio_pull_up(19);
    init_interrupts(); //interrupts

    // Initialize render area for entire frame (SSD1306_WIDTH pixels by SSD1306_NUM_PAGES pages), for clearing display
    struct render_area frame_area = {
        start_col: 0,
        end_col : SSD1306_WIDTH - 1,
        start_page : 0,
        end_page : SSD1306_NUM_PAGES - 1
        };

    calc_render_area_buflen(&frame_area);

    // clear display
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    while(1)
    {
        int y=0;
        int z=0;
        for (uint i = 0;i < count_of(letter); i++) {
            WriteChar(buf, y, 5,letter[i]);
            y+=8;
        }
        for (uint i = 0;i < count_of(message);i++)
        {
            WriteChar(buf,z,10,message[i]);
            z+=8;
        }
        render(buf, &frame_area);
        sleep_ms(500);
    }
}