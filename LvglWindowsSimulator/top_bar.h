#pragma once
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

    // TopBarContext → holds battery_fill obj and time_label
    typedef struct {
        lv_obj_t* battery_fill;
        lv_obj_t* time_label;
    } TopBarContext;

    // Create Top Bar → returns the context
    TopBarContext top_bar_create(lv_obj_t* parent);

    // Update battery %
    void top_bar_set_battery_percent(TopBarContext* ctx, int percent);

#ifdef __cplusplus
}
#endif
