#include "lvgl/lvgl.h"
#include "top_bar.h"
#include "now_playing_screen.h"
#include "audio.h"  // <== this is the correct include!

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include <windows.h>
#include <stdio.h>

#define MUSIC_FOLDER "C:\\Users\\Javie\\Music\\All My Music\\"
#define MAX_MUSIC_FILES 512
#define MAX_PATH_LEN 512

// Globals
static char music_files[MAX_MUSIC_FILES][MAX_PATH_LEN];
static int num_music_files = 0;
static int current_track_index = 0;

// At the top of the file (after the music_files global):
static lv_obj_t* time_left;
static lv_obj_t* time_right;
static lv_obj_t* progress;
static lv_timer_t* progress_timer;  // NEW → we will add a timer


// Forward declarations
static void back_btn_event_cb(lv_event_t* e);
static void play_pause_btn_event_cb(lv_event_t* e);
static void next_btn_event_cb(lv_event_t* e);
static void prev_btn_event_cb(lv_event_t* e);

static void scan_music_folder(void);


// === Now Playing Screen ===
void now_playing_screen_show(void)
{
    // First time → scan folder
    if (num_music_files == 0)
        scan_music_folder();

    // Load screen
    lv_obj_t* scr = lv_obj_create(NULL);
    lv_scr_load(scr);

    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    // Top Bar
    top_bar_create(scr);

    // Album Art container
    lv_obj_t* album_container = lv_obj_create(scr);
    lv_obj_set_size(album_container, 312, 110);
    lv_obj_align(album_container, LV_ALIGN_TOP_MID, 0, 24);
    lv_obj_set_style_bg_color(album_container, lv_color_hex(0xEEEEEE), 0);
    lv_obj_set_style_border_width(album_container, 1, 0);
    lv_obj_set_style_border_color(album_container, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_radius(album_container, 6, 0);

    // Back Button
    lv_obj_t* btn_back = lv_btn_create(album_container);
    lv_obj_set_size(btn_back, 24, 24);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 2, 2);
    lv_obj_t* label_back = lv_label_create(btn_back);
    lv_label_set_text(label_back, "<");
    lv_obj_center(label_back);
    lv_obj_add_event_cb(btn_back, back_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Album Art placeholder
    lv_obj_t* album_art = lv_obj_create(album_container);
    lv_obj_set_size(album_art, 100, 100);
    lv_obj_align(album_art, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(album_art, lv_color_hex(0xCCCCCC), 0);

    // Song Title
    lv_obj_t* song_title = lv_label_create(scr);
    lv_label_set_text(song_title, music_files[current_track_index]);
    lv_obj_set_style_text_font(song_title, &lv_font_montserrat_14, 0);
    lv_obj_align(song_title, LV_ALIGN_TOP_MID, 0, 150);

    // Artist (placeholder)
    lv_obj_t* artist = lv_label_create(scr);
    lv_label_set_text(artist, "Unknown Artist");
    lv_obj_set_style_text_font(artist, &lv_font_montserrat_12, 0);
    lv_obj_align(artist, LV_ALIGN_TOP_MID, 0, 165);

    // Progress Bar
    progress = lv_bar_create(scr);
    lv_obj_set_size(progress, 180, 8);
    lv_obj_align(progress, LV_ALIGN_BOTTOM_MID, 0, -90);
    lv_bar_set_value(progress, 0, LV_ANIM_OFF);

    // Time Left
    time_left = lv_label_create(scr);
    lv_label_set_text(time_left, "00:00");
    lv_obj_align_to(time_left, progress, LV_ALIGN_OUT_LEFT_MID, -8, 0);

    // Time Right
    time_right = lv_label_create(scr);
    lv_label_set_text(time_right, "??:??");
    lv_obj_align_to(time_right, progress, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Previous Button
    lv_obj_t* btn_prev = lv_btn_create(scr);
    lv_obj_set_size(btn_prev, 40, 40);
    lv_obj_align(btn_prev, LV_ALIGN_BOTTOM_LEFT, 80, -10);
    lv_obj_t* label_prev = lv_label_create(btn_prev);
    lv_label_set_text(label_prev, "<<");
    lv_obj_center(label_prev);
    lv_obj_add_event_cb(btn_prev, prev_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Play/Pause Button
    lv_obj_t* btn_play_pause = lv_btn_create(scr);
    lv_obj_set_size(btn_play_pause, 50, 50);
    lv_obj_align(btn_play_pause, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_t* label_play_pause = lv_label_create(btn_play_pause);
    lv_label_set_text(label_play_pause, "▶");
    lv_obj_center(label_play_pause);
    lv_obj_add_event_cb(btn_play_pause, play_pause_btn_event_cb, LV_EVENT_CLICKED, label_play_pause);

    // Next Button
    lv_obj_t* btn_next = lv_btn_create(scr);
    lv_obj_set_size(btn_next, 40, 40);
    lv_obj_align(btn_next, LV_ALIGN_BOTTOM_RIGHT, -80, -10);
    lv_obj_t* label_next = lv_label_create(btn_next);
    lv_label_set_text(label_next, ">>");
    lv_obj_center(label_next);
    lv_obj_add_event_cb(btn_next, next_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // === Start Playback ===
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, MAX_PATH_LEN, "%s%s", MUSIC_FOLDER, music_files[current_track_index]);
    audio_play(full_path);

    // Create timer to update progress
    progress_timer = lv_timer_create([](lv_timer_t* t) {

        int pos_ms = audio_get_position();
        int len_ms = audio_get_length();

        // Update time labels
        int pos_sec = pos_ms / 1000;
        int len_sec = len_ms / 1000;

        char buf_left[16];
        snprintf(buf_left, sizeof(buf_left), "%02d:%02d", pos_sec / 60, pos_sec % 60);
        lv_label_set_text(time_left, buf_left);

        char buf_right[16];
        snprintf(buf_right, sizeof(buf_right), "%02d:%02d", len_sec / 60, len_sec % 60);
        lv_label_set_text(time_right, buf_right);

        // Update progress bar
        int percent = (len_ms > 0) ? (pos_ms * 100) / len_ms : 0;
        lv_bar_set_value(progress, percent, LV_ANIM_OFF);

        }, 500, NULL);  // every 500 ms

}

// === Button Callbacks ===
static void back_btn_event_cb(lv_event_t* e)
{
    audio_stop();

    if (progress_timer) {
        lv_timer_del(progress_timer);
        progress_timer = NULL;
    }

    extern void ui_show_main_menu(void);
    ui_show_main_menu();
}

static void play_pause_btn_event_cb(lv_event_t* e)
{
    lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
    audio_toggle_pause();

    if (audio_is_playing())
        lv_label_set_text(label, "❚❚");
    else
        lv_label_set_text(label, "▶");
}

static void next_btn_event_cb(lv_event_t* e)
{
    audio_stop();

    if (progress_timer) {
        lv_timer_del(progress_timer);
        progress_timer = NULL;
    }

    current_track_index = (current_track_index + 1) % num_music_files;
    now_playing_screen_show();
}

static void prev_btn_event_cb(lv_event_t* e)
{
    audio_stop();

    if (progress_timer) {
        lv_timer_del(progress_timer);
        progress_timer = NULL;
    }

    current_track_index = (current_track_index - 1 + num_music_files) % num_music_files;
    now_playing_screen_show();
}


// === Scan Folder ===
static void scan_music_folder(void)
{
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;

    char search_path[MAX_PATH_LEN];
    snprintf(search_path, MAX_PATH_LEN, "%s*.mp3", MUSIC_FOLDER);

    hFind = FindFirstFileA(search_path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No files found.\n");
        return;
    }

    do {
        snprintf(music_files[num_music_files], MAX_PATH_LEN, "%s", findFileData.cFileName);
        num_music_files++;

        if (num_music_files >= MAX_MUSIC_FILES)
            break;

    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
}
