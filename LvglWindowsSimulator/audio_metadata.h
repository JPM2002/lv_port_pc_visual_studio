#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        char title[256];
        char artist[256];
        char album[256];
        char genre[128];
        unsigned int year;
        unsigned int track;
    } AudioMetadata;

    bool audio_get_metadata(const char* filePath, AudioMetadata* metadata);

#ifdef __cplusplus
}
#endif
