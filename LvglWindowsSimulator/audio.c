#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdbool.h>

// Global state
static ma_decoder decoder;
static ma_device device;
static bool isPlaying = false;

// NEW → track position in callback
volatile ma_uint64 g_lastPositionFrames = 0;
static ma_uint64 g_totalFrames = 0;  // Cache length once

// Data callback for device
static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL)
        return;

    ma_uint64 framesRead;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

    // Update global position safely
    g_lastPositionFrames += framesRead;

    if (framesRead < frameCount)
    {
        // If less frames were read → fill rest with silence
        ma_uint8* pRemaining = (ma_uint8*)pOutput + (framesRead * ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels));
        ma_uint64 remainingFrameCount = frameCount - framesRead;
        ma_silence_pcm_frames(pRemaining, remainingFrameCount, pDevice->playback.format, pDevice->playback.channels);
    }

    (void)pInput;
}

void audio_init(void)
{
    // Nothing needed for now
}

int audio_get_position()
{
    ma_uint64 sampleRate = decoder.outputSampleRate;

    // Just read the global position
    ma_uint64 posFrames = g_lastPositionFrames;

    double pos_sec = (double)posFrames / sampleRate;
    return (int)(pos_sec * 1000.0);  // ms
}

int audio_get_length()
{
    ma_uint64 sampleRate = decoder.outputSampleRate;

    // Use cached length
    double len_sec = (double)g_totalFrames / sampleRate;
    return (int)(len_sec * 1000.0);  // ms
}

void audio_play(const char* filePath)
{
    printf("Playing: %s\n", filePath);

    // Stop previous device if playing
    if (isPlaying)
    {
        ma_device_stop(&device);
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        isPlaying = false;
    }

    // Init decoder
    if (ma_decoder_init_file(filePath, NULL, &decoder) != MA_SUCCESS)
    {
        printf("Failed to load file: %s\n", filePath);
        return;
    }

    // Reset global playback state
    g_lastPositionFrames = 0;

    // Cache total frames
    g_totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &g_totalFrames);

    // Configure device
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;

    // Init device
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
    {
        printf("Failed to initialize device.\n");
        ma_decoder_uninit(&decoder);
        return;
    }

    // Start device
    if (ma_device_start(&device) != MA_SUCCESS)
    {
        printf("Failed to start device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    isPlaying = true;
}

void audio_toggle_pause(void)
{
    if (isPlaying)
    {
        // Pause = stop device but do not uninit
        ma_device_stop(&device);
        isPlaying = false;
    }
    else
    {
        ma_device_start(&device);
        isPlaying = true;
    }
}

void audio_stop(void)
{
    if (isPlaying)
    {
        ma_device_stop(&device);
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        isPlaying = false;
    }
}

bool audio_is_playing(void)
{
    return isPlaying;
}
