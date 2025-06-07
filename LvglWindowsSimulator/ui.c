#include "lvgl/lvgl.h"
#include "ui.h"
#include "top_bar.h"  // Include Top Bar

// Forward declarations
void ui_show_main_menu(void);
void ui_show_now_playing(void);
void ui_show_boot_screen(void);

// === Global TopBarContext ===
static TopBarContext global_top_bar_ctx;

// === Battery simulation (mock) ===
static int read_battery_percent(void)
{
    //static int mock_percent = 0;
    //mock_percent += 1;
    //if (mock_percent > 100) mock_percent = 0;
    //return mock_percent;
    return 80;
}

// === Battery update timer callback ===
static void battery_update_timer_cb(lv_timer_t* t)
{
    int percent = read_battery_percent();
    top_bar_set_battery_percent(&global_top_bar_ctx, percent);
}

// === C Callbacks ===
static void now_playing_btn_event_cb(lv_event_t* e)
{
    ui_show_now_playing();
}

static void boot_screen_timer_cb(lv_timer_t* t)
{
    ui_show_main_menu();
    lv_timer_del(t);
}

// === UI init ===
void ui_init(void)
{
    // Start battery update timer → runs every 1 sec
    lv_timer_create(battery_update_timer_cb, 1000, NULL);

    // First screen → show Boot Screen
    ui_show_boot_screen();
}

// === Boot Screen ===
void ui_show_boot_screen(void)
{
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    // Add Top Bar → assign to global
    global_top_bar_ctx = top_bar_create(scr);
    top_bar_set_battery_percent(&global_top_bar_ctx, read_battery_percent());  // <== ADD THIS

    // Logo / Title
    lv_obj_t* title = lv_label_create(scr);
    lv_label_set_text(title, "DIY iPod");
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -20);

    // Loading text
    lv_obj_t* loading = lv_label_create(scr);
    lv_label_set_text(loading, "Loading...");
    lv_obj_align(loading, LV_ALIGN_CENTER, 0, 30);

    // Timer → after 2 seconds → go to Main Menu
    lv_timer_t* timer = lv_timer_create_basic();
    lv_timer_set_cb(timer, boot_screen_timer_cb);
    lv_timer_set_period(timer, 2000);
}

// === Main Menu ===
void ui_show_main_menu(void)
{
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    // Add Top Bar → assign to global
    global_top_bar_ctx = top_bar_create(scr);
    top_bar_set_battery_percent(&global_top_bar_ctx, read_battery_percent());  // ADD THIS

    // Title label
    lv_obj_t* title = lv_label_create(scr);
    lv_label_set_text(title, "DIY iPod Player");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);  // move down a bit below Top Bar

    // Now Playing button
    lv_obj_t* btn_now_playing = lv_btn_create(scr);
    lv_obj_set_size(btn_now_playing, 150, 40);
    lv_obj_align(btn_now_playing, LV_ALIGN_CENTER, 0, -50);
    lv_obj_t* label_now_playing = lv_label_create(btn_now_playing);
    lv_label_set_text(label_now_playing, "Now Playing");
    lv_obj_add_event_cb(btn_now_playing, now_playing_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Browse Music button
    lv_obj_t* btn_browse = lv_btn_create(scr);
    lv_obj_set_size(btn_browse, 150, 40);
    lv_obj_align(btn_browse, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t* label_browse = lv_label_create(btn_browse);
    lv_label_set_text(label_browse, "Browse Music");

    // Settings button
    lv_obj_t* btn_settings = lv_btn_create(scr);
    lv_obj_set_size(btn_settings, 150, 40);
    lv_obj_align(btn_settings, LV_ALIGN_CENTER, 0, 50);
    lv_obj_t* label_settings = lv_label_create(btn_settings);
    lv_label_set_text(label_settings, "Settings");
}

// === Now Playing ===
void ui_show_now_playing(void)
{
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    // Add Top Bar → assign to global
    global_top_bar_ctx = top_bar_create(scr);

    // Title
    lv_obj_t* title = lv_label_create(scr);
    lv_label_set_text(title, "Now Playing");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);  // move down a bit below Top Bar

    // Album Art placeholder
    lv_obj_t* album_art = lv_obj_create(scr);
    lv_obj_set_size(album_art, 100, 100);
    lv_obj_align(album_art, LV_ALIGN_TOP_LEFT, 20, 70);

    // Song Title
    lv_obj_t* song_title = lv_label_create(scr);
    lv_label_set_text(song_title, "Song Title - Example");
    lv_obj_align(song_title, LV_ALIGN_TOP_LEFT, 140, 80);

    // Artist
    lv_obj_t* artist = lv_label_create(scr);
    lv_label_set_text(artist, "Artist Name");
    lv_obj_align(artist, LV_ALIGN_TOP_LEFT, 140, 110);

    // Progress Bar
    lv_obj_t* progress = lv_bar_create(scr);
    lv_obj_set_size(progress, 200, 10);
    lv_obj_align(progress, LV_ALIGN_BOTTOM_MID, 0, -80);
    lv_bar_set_value(progress, 30, LV_ANIM_OFF); // Example 30%

    // Prev button
    lv_obj_t* btn_prev = lv_btn_create(scr);
    lv_obj_set_size(btn_prev, 50, 50);
    lv_obj_align(btn_prev, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_t* label_prev = lv_label_create(btn_prev);
    lv_label_set_text(label_prev, "<<");

    // Play/Pause button
    lv_obj_t* btn_play_pause = lv_btn_create(scr);
    lv_obj_set_size(btn_play_pause, 50, 50);
    lv_obj_align(btn_play_pause, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t* label_play_pause = lv_label_create(btn_play_pause);
    lv_label_set_text(label_play_pause, "||");

    // Next button
    lv_obj_t* btn_next = lv_btn_create(scr);
    lv_obj_set_size(btn_next, 50, 50);
    lv_obj_align(btn_next, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_t* label_next = lv_label_create(btn_next);
    lv_label_set_text(label_next, ">>");
}
