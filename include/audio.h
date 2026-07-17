#include <stdio.h>
#include <stdlib.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void start_audio();
void end_audio();

typedef struct {
    short *data;
    size_t bsize;
} audio;

audio load_audio(char path[]) {
    audio aud;

    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);
    aud.bsize = len;

    aud.data = (short*)malloc(len*sizeof(short));
    fread(aud.data, sizeof(short), len, file);

    fclose(file);

    return aud;
}

void play_audio(audio aud);

#ifdef _WIN32
// No audio for windows :P
#else
#include <alsa/asoundlib.h>

static snd_pcm_t *device;

void start_audio() {
    snd_pcm_open(&device, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0);

    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_malloc(&params);

    snd_pcm_hw_params_any(device, params);

    snd_pcm_hw_params_set_rate_resample(device, params, 1);
    snd_pcm_hw_params_set_access(device, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(device, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(device, params, 2);

    unsigned int rate = 44100;
    snd_pcm_hw_params_set_rate_near(device, params, &rate, 0);

    snd_pcm_hw_params(device, params);

    snd_pcm_uframes_t bsize;
    snd_pcm_hw_params_get_buffer_size(params, &bsize);

    snd_pcm_hw_params_free(params);

    snd_pcm_prepare(device);
}

void end_audio() {
    snd_pcm_close(device);
}

void play_audio(audio aud) {
    snd_pcm_writei(device, aud.data, aud.bsize);
}
#endif

#ifdef __cplusplus
}
#endif