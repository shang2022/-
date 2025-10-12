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

#define MIN_SETTING_IDX 0
#define MAX_SETTING_IDX 9

#define SCREEN_X_OFFSET_IDX 4
#define SCREEN_Y_OFFSET_IDX 5

#define TEMP_ADJ_LOW_IDX 6
#define TEMP_ADJ_HIGH_IDX 7

#define SETTING_EC11_DIR_IDX 8
#define SETTING_INV_COLOR_IDX 9

static void show_setting(void) {
    //
    LCD_show_chinese(20, 25, 26 + _select_idx * 2, 2, WHITE);
}

static void show_value(void) {
    //
    int8_t v = ((int8_t *)&g_config)[_select_idx];
    if (_select_idx == TEMP_ADJ_LOW_IDX || _select_idx == TEMP_ADJ_HIGH_IDX) {
        v -= 30;
    }
    LCD_show_number(75, 25, v, 3, WHITE);
}

void SET_init(void) {
    //
}

void SET_enter(void) {
    _setting = 0;
    _select_idx = 0;

    EC11_set_range(0, MIN_SETTING_IDX, MAX_SETTING_IDX, 1);
    LCD_clear();
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
            uint16_t max_val = 127;
            int8_t loop = 0;
            if (_select_idx == TEMP_ADJ_LOW_IDX || _select_idx == TEMP_ADJ_HIGH_IDX) {
                max_val = 60; // -30 ~ +30
            } else if (_select_idx >= SETTING_EC11_DIR_IDX) {
                max_val = 1; // 0/1
                loop = 1;
            }
            EC11_set_range(values[_select_idx], 0, max_val, loop);
            LCD_fill(0, 25, 8, 30, GREEN);
        }
    } else {
        if (values[_select_idx] != g_ec11_value) {
            values[_select_idx] = g_ec11_value;

            // X/Y偏移
            if (_select_idx == SCREEN_X_OFFSET_IDX || _select_idx == SCREEN_Y_OFFSET_IDX) {
                LCD_clear();
                show_setting();
                LCD_fill(0, 25, 8, 30, GREEN);
            }
            show_value();
        }

        if (EC11_is_button_pressed()) {
            _setting = 0;
            LCD_fill(0, 25, 8, 30, BLACK);

            EEPROM_save_cfg();
            EC11_set_range(_select_idx, MIN_SETTING_IDX, MAX_SETTING_IDX, 1);

            if (_select_idx == SETTING_INV_COLOR_IDX) {
                LCD_update_color_inv();
            }
        }
    }

    if (HEAT_is_back_pressed()) {
        g_app_stage = STAGE_MENU;
        MENU_enter();
    }
}
