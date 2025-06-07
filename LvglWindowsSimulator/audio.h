#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    void audio_init(void);
    void audio_play(const char* filePath);
    void audio_stop(void);
    void audio_toggle_pause(void);

#ifdef __cplusplus
}
#endif
