#include "lvgl/lvgl.h"
#include "top_bar.h"
#include <time.h>
#include <stdio.h>

static TopBarContext* global_time_ctx = NULL;  // Fallback global pointer
static lv_timer_t* time_update_timer = NULL;

// Internal helper to update time label
static void update_time_label(lv_obj_t* label)
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[6];  // HH:MM + \0

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%H:%M", timeinfo);
    lv_label_set_text(label, buffer);
}

// Timer callback
static void time_update_timer_cb(lv_timer_t* t)
{
    if (global_time_ctx && global_time_ctx->time_label)
    {
        update_time_label(global_time_ctx->time_label);
    }
}

TopBarContext top_bar_create(lv_obj_t* parent)
{
    TopBarContext ctx = { 0 };  // initialize context

    // Create container
    lv_obj_t* bar = lv_obj_create(parent);
    lv_obj_set_size(bar, 320, 20);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(bar, 0, 0);

    // Add bottom border (debug only)
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_border_color(bar, lv_color_hex(0xAAAAAA), 0);  // light gray line

    // Disable scroll
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    // Battery outline
    lv_obj_t* battery_outline = lv_obj_create(bar);
    lv_obj_remove_style_all(battery_outline);  // <--- SUPER important for clean rectangle!

    lv_obj_set_size(battery_outline, 30, 10);
    lv_obj_align(battery_outline, LV_ALIGN_RIGHT_MID, -2, 0);
    lv_obj_set_style_border_width(battery_outline, 2, 0);
    lv_obj_set_style_border_color(battery_outline, lv_color_hex(0x000000), 0);
    lv_obj_set_style_radius(battery_outline, 0, 0);
    lv_obj_set_style_bg_color(battery_outline, lv_color_hex(0xFFFFFF), 0);
    lv_obj_clear_flag(battery_outline, LV_OBJ_FLAG_SCROLLABLE);
    // DO NOT add LV_OBJ_FLAG_CLIP_CHILDREN


    // Battery fill → store in context
    ctx.battery_fill = lv_obj_create(battery_outline);
    lv_obj_set_size(ctx.battery_fill, 0, 6);  // Start 0%
    lv_obj_align(ctx.battery_fill, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(ctx.battery_fill, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(ctx.battery_fill, 0, 0);
    lv_obj_set_style_radius(ctx.battery_fill, 0, 0);  // force sharp fill
    lv_obj_set_style_clip_corner(ctx.battery_fill, false, 0);  // force disable clip

    // Battery head
    lv_obj_t* battery_head = lv_obj_create(bar);
    lv_obj_set_size(battery_head, 2, 6);
    lv_obj_align_to(battery_head, battery_outline, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
    lv_obj_set_style_bg_color(battery_head, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(battery_head, 0, 0);

    // Time label (left)
    ctx.time_label = lv_label_create(bar);
    lv_obj_align(ctx.time_label, LV_ALIGN_LEFT_MID, 2, 0);
    update_time_label(ctx.time_label);  // Initialize text

    // Start or update the timer
    global_time_ctx = &ctx;
    if (time_update_timer == NULL)
    {
        time_update_timer = lv_timer_create(time_update_timer_cb, 60000, NULL);  // 1 min
    }

    return ctx;
}

void top_bar_set_battery_percent(TopBarContext* ctx, int percent)
{
    if (ctx == NULL || ctx->battery_fill == NULL)
        return;

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int max_fill_width = 26;
    int new_width = (percent * max_fill_width) / 100;

    lv_obj_set_width(ctx->battery_fill, new_width);

    // Optional: Color based on % level
    if (percent <= 20)
        lv_obj_set_style_bg_color(ctx->battery_fill, lv_color_hex(0xFF0000), 0);  // Red
    else if (percent <= 50)
        lv_obj_set_style_bg_color(ctx->battery_fill, lv_color_hex(0xFFFF00), 0);  // Yellow
    else
        lv_obj_set_style_bg_color(ctx->battery_fill, lv_color_hex(0x00FF00), 0);  // Green
}
