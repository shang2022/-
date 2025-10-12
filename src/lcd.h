#ifndef __H_LCD__
#define __H_LCD__

#include "fw_hal.h"

#define SPI_SDI P13
#define SPI_SCK P15
#define SPI_DC P17
#define SPI_RST P16
#define SPI_CS P12

#define LCD_WIDTH 160
#define LCD_HEIGHT 80

#define BLACK 0
#define WHITE 1
#define GREEN 2
#define RED 3
#define BLUE 4

#define GRAY_BG 10
#define BLACK_GRAY 10
#define WHITE_GRAY 11
#define RED_GRAY 12
#define GREEN_GRAY 13
#define BLUE_GRAY 14

void LCD_init(void);
void LCD_update_color_inv(void);
void LCD_clear(void);
void LCD_fill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void LCD_show_font_char(uint8_t x, uint8_t y, const uint8_t *font, uint8_t bytes, uint8_t color);
void LCD_show_number(uint8_t x, uint8_t y, int16_t number, uint8_t width, uint8_t color);
void LCD_show_chinese(uint8_t x, uint8_t y, const uint8_t text_idx, uint8_t count, uint8_t color);

#endif
