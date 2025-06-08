#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    void audio_init(void);
    void audio_play(const char* filePath);
    void audio_stop(void);
    void audio_toggle_pause(void);
    bool audio_is_playing(void);
    int audio_get_position();  // in ms
    int audio_get_length();    // in ms


#ifdef __cplusplus
}
#endif
