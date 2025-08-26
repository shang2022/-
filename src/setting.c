#include "setting.h"
#include "app.h"
#include "ec11.h"
#include "eeprom.h"
#include "font.h"
#include "heat.h"
#include "lcd.h"
#include "menu.h"

static uint8_t _setting = 0;
static uint8_t _select_idx = 0;

static void show_setting(void) {
    //
    LCD_show_chinese(20, 25, 26 + _select_idx * 2, 2, WHITE);
}

static void show_value(void) {
    //
    uint8_t v = ((int8_t *)&g_config)[_select_idx];
    LCD_show_number(75, 25, (uint16_t)v, 3, WHITE);
}

void SET_init(void) {
    //
}

void SET_enter(void) {
    _setting = 0;
    _select_idx = 0;

    EC11_set_range(0, 0, 7, 1);
    LCD_clear(BLACK);
    show_setting();
    show_value();
}

void SET_run(void) {
    int8_t *values = (int8_t *)&g_config;
    if (_setting == 0) {
        if (_select_idx != g_ec11_value) {
            _select_idx = g_ec11_value;
            show_setting();
            show_value();
        }

        if (EC11_is_button_pressed()) {
            _setting = 1;
            EC11_set_range(values[_select_idx], 0, _select_idx >= 6 ? 1 : 255, _select_idx >= 6 ? 1 : 0);
            LCD_fill(0, 25, 8, 30, GREEN);
        }
    } else {
        if (values[_select_idx] != g_ec11_value) {
            values[_select_idx] = g_ec11_value;

            if (_select_idx == 4 || _select_idx == 5) { // X/Y偏移
                LCD_clear(BLACK);
                show_setting();
                LCD_fill(0, 25, 8, 30, GREEN);
            }
            show_value();
        }

        if (EC11_is_button_pressed()) {
            _setting = 0;
            LCD_fill(0, 25, 8, 30, BLACK);

            EEPROM_save_cfg();
            EC11_set_range(_select_idx, 0, 7, 1);

            if (_select_idx == 7) {
                LCD_update_color_inv();
            }
        }
    }

    if (HEAT_is_back_pressed()) {
        g_app_stage = STAGE_MENU;
        MENU_enter();
    }
}
