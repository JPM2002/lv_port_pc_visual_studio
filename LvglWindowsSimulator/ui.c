#include "lvgl/lvgl.h"
#include "ui.h"
#include "top_bar.h"
#include "now_playing_screen.h"


// Forward declarations
void ui_show_main_menu(void);
void ui_show_boot_screen(void);

// === Global TopBarContext ===
static TopBarContext global_top_bar_ctx;

// === Battery simulation (mock) ===
static int read_battery_percent(void)
{
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
    now_playing_screen_show();
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
    top_bar_set_battery_percent(&global_top_bar_ctx, read_battery_percent());

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

    // Add Top Bar
    global_top_bar_ctx = top_bar_create(scr);
    top_bar_set_battery_percent(&global_top_bar_ctx, read_battery_percent());

    // Create List
    lv_obj_t* list = lv_list_create(scr);
    lv_obj_set_size(list, 312, 180);  // full width, adjust height to fit screen below Top Bar
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 24);  // below top bar (TopBar height ≈ 24px)

    // OPTIONAL → Style the selected item (yellow highlight)
    static lv_style_t style_selected;
    lv_style_init(&style_selected);
    lv_style_set_bg_color(&style_selected, lv_color_hex(0xFFD700));  // Yellow
    lv_style_set_text_color(&style_selected, lv_color_hex(0x000000));  // Black text

    // Add List Buttons
    lv_obj_t* btn_files = lv_list_add_btn(list, LV_SYMBOL_FILE, "Files");
    lv_obj_add_event_cb(btn_files, now_playing_btn_event_cb, LV_EVENT_CLICKED, NULL);  // TEMP action

    lv_obj_t* btn_database = lv_list_add_btn(list, LV_SYMBOL_AUDIO, "Database");

    lv_obj_t* btn_resume = lv_list_add_btn(list, LV_SYMBOL_PLAY, "Resume Playback");

    lv_obj_t* btn_settings = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "Settings");

    lv_obj_t* btn_recording = lv_list_add_btn(list, LV_SYMBOL_VIDEO, "Recording");

    lv_obj_t* btn_fm_radio = lv_list_add_btn(list, LV_SYMBOL_BELL, "FM Radio");

    lv_obj_t* btn_playlists = lv_list_add_btn(list, LV_SYMBOL_LIST, "Playlists");

    lv_obj_t* btn_plugins = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, "Plugins");  // better icon

    lv_obj_t* btn_system = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "System");

    // OPTIONAL: Simulate "selection" → style the first item
    lv_obj_add_style(btn_files, &style_selected, LV_PART_MAIN);
}
