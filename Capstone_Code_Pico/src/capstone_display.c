#include "capstone_display.h"
#include <stdio.h>
#include <string.h>
#include "../pico/stdlib.h"
#include "../hardware/i2c.h"
#include <stdint.h>

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define CHAR_SIZE 6

#define OLED_ADDR 0x3C

char message[21];
uint8_t buffer[6];

const uint8_t ssd1306xled_font6x8 []={
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // space
  0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
  0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
  0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
  0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
  0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
};
const char alphabet[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ-.";
void display_message()
{
    uint8_t len = strlen(message);
    uint8_t cursor_col = 0;

    //Start at top-left always.
    oled_set_cursor(0, 0);

    for (uint8_t j = 0; j < len; j++)
    {
        char current_letter = message[j];

        //Find matching font index
        uint8_t idx = 0xFF;
        for (uint8_t i = 0; i < 29; i++)
        {
            if (alphabet[i] == current_letter)
            {
                idx = i;
                break;
            }
        }

        //Skip unknown characters (redundant error checking)
        if (idx == 0xFF) continue;

        //Load 6 bytes of font data
        for (uint8_t k = 0; k < 6; k++)
            buffer[k] = ssd1306xled_font6x8[idx * CHAR_SIZE + k];

        //Write to screen
        oled_write_char6(buffer);

        //Move cursor to next char
        cursor_col += CHAR_SIZE;
        oled_set_cursor(0, cursor_col);
    }
}
void i2c_init(void)
{
    stdio_init_all();

    //init at 100kHz
    i2c_init(I2C_PORT, 100*1000);

    //configure GPIOs 0 and 1 to be I2C pins
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
}
void oled_send_command(uint8_t cmd)
{
    uint8_t buffer[2];
    buffer[0] = 0x00; //control byte
    buffer[1] = cmd; //pass actual command
    i2c_write_blocking(I2C_PORT,OLED_ADDR,buffer,2,false);
}
void oled_init(void)
{
    oled_send_command(0xAE); // display OFF
    oled_send_command(0xD5); // Set display clock divide
    oled_send_command(0x80); // Suggested ratio
    oled_send_command(0xA8); // Set multiplex
    oled_send_command(0x1F); // 0x1F = 31 → for 128x32
    oled_send_command(0xD3); // Display offset
    oled_send_command(0x00); // 0 offset
    oled_send_command(0x40); // Set display start line to 0
    oled_send_command(0x8D); // Charge pump
    oled_send_command(0x14); // Enable charge pump
    oled_send_command(0x20); // Memory mode
    oled_send_command(0x00); // Horizontal addressing mode
    oled_send_command(0xA1); // Segment remap (mirror horizontally)
    oled_send_command(0xC8); // COM scan direction (remap vertically)
    oled_send_command(0xDA); // Set COM pins config
    oled_send_command(0x02); // *128x32 specific* (NOT 0x12!)
    oled_send_command(0x81); // Contrast
    oled_send_command(0x8F);
    oled_send_command(0xD9); // Pre-charge
    oled_send_command(0xF1);
    oled_send_command(0xDB); // VCOM detect
    oled_send_command(0x40);
    oled_send_command(0xA4); // Display all on (resume RAM)
    oled_send_command(0xA6); // Normal display (not inverted)
    oled_send_command(0x2E); // Deactivate scroll (safety)
    oled_send_command(0xAF); // Display ON
}
void oled_send_data(uint8_t *data, uint8_t len)
{
    uint8_t buffer[len];
    buffer[0] = 0x40; //control byte
    for (uint8_t i = 0; i < len; i++) buffer[i] = data[i];
    i2c_write_blocking(I2C_PORT,OLED_ADDR,buffer,len,false);
}
void oled_set_cursor(uint8_t page, uint8_t column)
{
    oled_send_command(0xB0 | (page & 0x0F));     // page
    oled_send_command(0x00 | (column & 0x0F));   // lower col
    oled_send_command(0x10 | (column >> 4));     // upper col
}
void oled_write_char6(const uint8_t data[6])
{
    oled_send_data(data, 6);
}
void oled_clear(void)
{
    for (uint8_t page = 0; page < 4; page++) { // 128x32 = 4 pages
        oled_set_cursor(page, 0);
        for (uint8_t i = 0; i < 128; i++) {
            uint8_t zero = 0;
            oled_send_data(&zero, 1);
        }
    }
    oled_set_cursor(0, 0);
}
void test()
{

}