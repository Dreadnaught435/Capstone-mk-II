#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "capstone_display.h"
#include "capstone_input.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "raspberry26x32.h"
#include "ssd1306_font.h"

int main() {
    stdio_init_all();

    // I2C is "open drain", pull ups to keep signal high when no data is being
    // sent
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    //inits
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

    // zero the entire display
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    // char *text[] = {"penis","sinep"};

    // int y = 0;
    // for (uint i = 0;i < count_of(text); i++) {
    //     WriteString(buf, 5, y, text[i]);
    //     y+=8;
    // }
    // render(buf, &frame_area);
    // sleep_ms(500);
    // memset(buf, 0, SSD1306_BUF_LEN);
    // render(buf, &frame_area);

    //strcpy(test,"fart");
    // test[0] = 'a';
    // test[1] = 'g';
    while(1)
    {
        int y=0;
        for (uint i = 0;i < count_of(test); i++) {
            WriteChar(buf, y, 5, test[i]);
            y+=8;
        }
        render(buf, &frame_area);
        sleep_ms(500);
    }
}