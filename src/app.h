
#ifndef __H_APP__
#define __H_APP__

#include "fw_sys.h"

#define STAGE_MENU 0
#define STAGE_REFLOW 1
#define STAGE_CONST_TEMP 2
#define STAGE_SETTING 3

extern int8_t g_app_stage;
extern uint8_t g_sec_elapsed;
extern volatile uint8_t g_time_sec;

struct CONFIGURE {
    int8_t kp;
    int8_t ki;
    int8_t kd;
    int8_t delay;
    int8_t x_offset;
    int8_t y_offset;
    int8_t ec11_dir;
    int8_t color_inv;
};
extern struct CONFIGURE g_config;

#endif
