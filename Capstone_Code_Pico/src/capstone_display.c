#include "capstone_display.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdint.h>

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define OLED_ADDR 0x3C 

#define CHAR_SIZE 6

#define SSD1306_HEIGHT             64
#define SSD1306_WIDTH               128

#define SSD1306_SET_MEM_MODE        _u(0x20)
#define SSD1306_SET_COL_ADDR        _u(0x21)
#define SSD1306_SET_PAGE_ADDR       _u(0x22)
#define SSD1306_SET_HORIZ_SCROLL    _u(0x26)
#define SSD1306_SET_SCROLL          _u(0x2E)

#define SSD1306_SET_DISP_START_LINE _u(0x40)

#define SSD1306_SET_CONTRAST        _u(0x81)
#define SSD1306_SET_CHARGE_PUMP     _u(0x8D)

#define SSD1306_SET_SEG_REMAP       _u(0xA0)
#define SSD1306_SET_ENTIRE_ON       _u(0xA4)
#define SSD1306_SET_ALL_ON          _u(0xA5)
#define SSD1306_SET_NORM_DISP       _u(0xA6)
#define SSD1306_SET_INV_DISP        _u(0xA7)
#define SSD1306_SET_MUX_RATIO       _u(0xA8)
#define SSD1306_SET_DISP            _u(0xAE)
#define SSD1306_SET_COM_OUT_DIR     _u(0xC0)
#define SSD1306_SET_COM_OUT_DIR_FLIP _u(0xC0)

#define SSD1306_SET_DISP_OFFSET     _u(0xD3)
#define SSD1306_SET_DISP_CLK_DIV    _u(0xD5)
#define SSD1306_SET_PRECHARGE       _u(0xD9)
#define SSD1306_SET_COM_PIN_CFG     _u(0xDA)
#define SSD1306_SET_VCOM_DESEL      _u(0xDB)

#define SSD1306_PAGE_HEIGHT         _u(8)
#define SSD1306_NUM_PAGES           (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN             (SSD1306_NUM_PAGES * SSD1306_WIDTH)

#define SSD1306_WRITE_MODE         _u(0xFE)
#define SSD1306_READ_MODE          _u(0xFF)



uint8_t message[21];
uint8_t buffer[64];

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
void start_i2c(void)
{
    stdio_init_all();

    //init at 100kHz
    i2c_init(I2C_PORT, 100*1000);

    //configure GPIOs 0 and 1 to be I2C pins
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
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
    uint8_t cmds[] = {
        SSD1306_SET_DISP,               // set display off
        /* memory mapping */
        SSD1306_SET_MEM_MODE,           // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
        0x00,                           // horizontal addressing mode
        /* resolution and layout */
        SSD1306_SET_DISP_START_LINE,    // set display start line to 0
        SSD1306_SET_SEG_REMAP | 0x01,   // set segment re-map, column address 127 is mapped to SEG0
        SSD1306_SET_MUX_RATIO,          // set multiplex ratio
        SSD1306_HEIGHT - 1,             // Display height - 1
        SSD1306_SET_COM_OUT_DIR | 0x08, // set COM (common) output scan direction. Scan from bottom up, COM[N-1] to COM0
        SSD1306_SET_DISP_OFFSET,        // set display offset
        0x00,                           // no offset
        SSD1306_SET_COM_PIN_CFG,        // set COM (common) pins hardware configuration. Board specific magic number.
                                        // 0x02 Works for 128x32, 0x12 Possibly works for 128x64. Other options 0x22, 0x32
#if ((SSD1306_WIDTH == 128) && (SSD1306_HEIGHT == 32))
        0x02,
#elif ((SSD1306_WIDTH == 128) && (SSD1306_HEIGHT == 64))
        0x12,
#else
        0x02,
#endif
        /* timing and driving scheme */
        SSD1306_SET_DISP_CLK_DIV,       // set display clock divide ratio
        0x80,                           // div ratio of 1, standard freq
        SSD1306_SET_PRECHARGE,          // set pre-charge period
        0xF1,                           // Vcc internally generated on our board
        SSD1306_SET_VCOM_DESEL,         // set VCOMH deselect level
        0x30,                           // 0.83xVcc
        /* display */
        SSD1306_SET_CONTRAST,           // set contrast control
        0xFF,
        SSD1306_SET_ENTIRE_ON,          // set entire display on to follow RAM content
        SSD1306_SET_NORM_DISP,           // set normal (not inverted) display
        SSD1306_SET_CHARGE_PUMP,        // set charge pump
        0x14,                           // Vcc internally generated on our board
        SSD1306_SET_SCROLL | 0x00,      // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
        SSD1306_SET_DISP | 0x01, // turn display on
    };

    for(int i=0;i<sizeof(cmds)/sizeof(cmds[1]);i++)
    {
        oled_send_command(cmds[i]);
    }
}
void oled_send_data(uint8_t *data, uint8_t len)
{
    uint8_t buffer[len];
    buffer[0] = 0x40; //control byte
    for (uint8_t i = 1; i < len; i++) buffer[i] = data[i];
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
    for (uint8_t page = 0; page < 8; page++) { // 128x32 = 4 pages
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
    strcpy(message, "PENIS BALLS");
	display_message();
    oled_clear();
	return;
}