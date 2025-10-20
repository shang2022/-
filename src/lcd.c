#include "lcd.h"
#include "app.h"
#include "eeprom.h"
#include "font.h"
#include "fw_hal.h"

#define CMD_SET_X 0x2A  // 设置X坐标
#define CMD_SET_Y 0x2B  // 设置Y坐标
#define CMD_WR_RAM 0x2C // 写GRAM

#define RGB565(r, g, b) ((uint16_t)((((uint8_t)(r) >> 3) << 11) | (((uint8_t)(g) >> 2) << 5) | ((uint8_t)(b) >> 3)))

#define COLOR_WHITE RGB565(255, 255, 255)
#define COLOR_BLACK RGB565(0, 0, 0)
#define COLOR_BLUE RGB565(0, 0, 255)
#define COLOR_GREEN RGB565(0, 255, 0)
#define COLOR_RED RGB565(255, 0, 0)
#define COLOR_GRAY RGB565(0x00, 0xff, 0x00)

#define FONT_HEIGHT 26

static uint16_t COLOR_MAP[] = {COLOR_BLACK, COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_BLUE};

static uint8_t font_bmp[52];

#define CHINESE_WIDTH 26
#define CHINESE_BYTES 85

#define DIGIT_WIDTH 16
#define DIGIT_BYTES 52

static void writeBus(uint8_t data) {
    for (uint8_t i = 0; i < 8; ++i) {
        SPI_SCK = 0;
        if (data & 0x80) {
            SPI_SDI = 1;
        } else {
            SPI_SDI = 0;
        }
        SPI_SCK = 1;
        data <<= 1;
    }
}

static void LCD_WR_REG(uint8_t reg) {
    SPI_DC = 0;
    SPI_CS = 0;
    writeBus(reg);
    SPI_CS = 1;
}

static void LCD_WR_DATA(uint8_t data) {
    SPI_DC = 1;
    SPI_CS = 0;
    writeBus(data);
    SPI_CS = 1;
}

// 开始写GRAM
static void writeRAMPrepare(void) { LCD_WR_REG(CMD_WR_RAM); }

void setWindow(uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height) {
    LCD_WR_REG(CMD_SET_X);
    LCD_WR_DATA(0);
    LCD_WR_DATA(y_start + g_config.y_offset);
    LCD_WR_DATA(0);
    LCD_WR_DATA(y_start + height - 1 + g_config.y_offset);
    LCD_WR_REG(CMD_SET_Y);
    LCD_WR_DATA(0);
    LCD_WR_DATA(x_start + g_config.x_offset);
    LCD_WR_DATA(0);
    LCD_WR_DATA(x_start + width - 1 + g_config.x_offset);
}

static void drawPoint(uint16_t color) {
    writeBus(color >> 8);
    writeBus(color & 0xFF);
}

// 初始化lcd
void LCD_init(void) {
    // SPI_SetClockPrescaler(SPI_ClockPreScaler_4);
    // SPI_SetClockPolarity(HAL_State_ON);
    // SPI_SetClockPhase(SPI_ClockPhase_LeadingEdge);
    // SPI_SetDataOrder(SPI_DataOrder_MSB);
    // SPI_IgnoreSlaveSelect(HAL_State_OFF);
    // SPI_SetMasterMode(HAL_State_ON);
    // SPI_SetEnabled(HAL_State_ON);
    SPCTL = 0x58;

    // SPI_SetPort(SPI_AlterPort_P12P54_P13_P14_P15);
    P_SW1 = 0x00;

    SPI_RST = 1;
    SYS_Delay(300);

    LCD_WR_REG(0x11); // Sleep out

    SYS_Delay(120); // delay_ms 120ms

    LCD_WR_REG(0xB1); // Normal mode
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x3C);

    // LCD_WR_REG(0xB2); // Idle mode
    // LCD_WR_DATA(0x05);
    // LCD_WR_DATA(0x3C);
    // LCD_WR_DATA(0x3C);

    // LCD_WR_REG(0xB3); // Partial mode
    // LCD_WR_DATA(0x05);
    // LCD_WR_DATA(0x3C);
    // LCD_WR_DATA(0x3C);
    // LCD_WR_DATA(0x05);
    // LCD_WR_DATA(0x3C);
    // LCD_WR_DATA(0x3C);

    LCD_WR_REG(0xB4); // Dot inversion
    LCD_WR_DATA(0x03);

    LCD_WR_REG(0xC0); // AVDD GVDD
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x84);

    LCD_WR_REG(0xC1); // VGH VGL
    LCD_WR_DATA(0x06);

    LCD_WR_REG(0xC2); // Normal Mode
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);

    // LCD_WR_REG(0xC3); // Idle
    // LCD_WR_DATA(0x8D);
    // LCD_WR_DATA(0x6A);

    // LCD_WR_REG(0xC4); // Partial+Full
    // LCD_WR_DATA(0x8D);
    // LCD_WR_DATA(0xEE);

    LCD_WR_REG(0xC5); // VCOM
    LCD_WR_DATA(0x3A);

    LCD_WR_REG(0xE0); // positive gamma
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x1D);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);

    LCD_WR_REG(0xE1); // negative gamma
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x1F);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x05);

    // 0度旋转
    LCD_WR_REG(0x36);
    LCD_WR_DATA((uint8_t)((1 << 3) | (1 << 7) | (0 << 6) | (0 << 5)));

    LCD_update_color_inv();
    LCD_WR_REG(0x29); // Display on
}

void LCD_update_color_inv(void) {
    LCD_WR_REG(g_config.color_inv ? 0x21 : 0x20); // Display inversion
}

void LCD_clear(void) { LCD_fill(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK); }

void LCD_fill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
    setWindow(x, y, width, height);
    writeRAMPrepare();
    SPI_DC = 1;
    SPI_CS = 0;
    for (uint8_t x = 0; x < width; ++x) {
        for (uint8_t y = 0; y < height; ++y) {
            drawPoint(COLOR_MAP[color]);
        }
    }
    SPI_CS = 1;
}

void LCD_show_font_char(const uint8_t *font, uint8_t bytes, uint8_t color) {
    for (uint8_t b = 0; b < bytes; b++) {
        uint8_t c = font[b];
        for (uint8_t i = 0; i < 8; i++) {
            if (c & 0x80) {
                drawPoint(COLOR_MAP[color > 10 ? color - 10 : color]);
            } else {
                drawPoint(color >= 10 ? COLOR_GRAY : COLOR_BLACK);
            }
            c <<= 1;
        }
    }
}

void LCD_show_number(uint8_t x, uint8_t y, int16_t n, uint8_t width, uint8_t color) {
    uint16_t number = (uint16_t)(n >= 0 ? n : -n);
    x = x + DIGIT_WIDTH * (width - 1);
    uint16_t v = number;
    while (width > 0) {
        uint8_t digit = number % 10;
        number /= 10;

        if (number == 0 && digit == 0 && v != 0) {
            if (n < 0 && v == 1) {
                digit = 10; // '-'
                n = 0;
                goto __draw;
            }
            LCD_fill(x, y, DIGIT_WIDTH, FONT_HEIGHT, BLACK);
        } else {
        __draw:
            EEPROM_read(FONT_DIGIT_ADDR + digit * DIGIT_BYTES, font_bmp, DIGIT_BYTES);

            setWindow(x, y, CHINESE_WIDTH, FONT_HEIGHT);
            writeRAMPrepare();
            SPI_DC = 1;
            SPI_CS = 0;
            LCD_show_font_char(font_bmp, DIGIT_BYTES, color);
            SPI_CS = 1;

            v = 1;
        }
        width--;
        x -= DIGIT_WIDTH;
    }
}

void LCD_show_chinese(uint8_t x, uint8_t y, const uint8_t text_idx, uint8_t count, uint8_t color) {
    for (uint8_t i = 0; i < count; i++) {
        setWindow(x, y, CHINESE_WIDTH, FONT_HEIGHT);
        writeRAMPrepare();
        SPI_DC = 1;
        SPI_CS = 0;

        uint8_t idx = TEXT_IDX_ADDR[text_idx + i];

        EEPROM_read(FONT_CHINESE_ADDR + idx * CHINESE_BYTES, font_bmp, 52);
        LCD_show_font_char(font_bmp, 52, color);

        EEPROM_read(FONT_CHINESE_ADDR + idx * CHINESE_BYTES + 52, font_bmp, 33);
        LCD_show_font_char(font_bmp, 33, color);

        x += CHINESE_WIDTH;
        SPI_CS = 1;
    }
}
