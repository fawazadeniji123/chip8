#include "generate_sound.h"
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PCM_DEVICE "default"
#define PI 3.14159265358979

static void generate_sine_wave(int16_t *buffer, int samples, int frequency,
                               int sample_rate) {
  for (int i = 0; i < samples; i++) {
    buffer[i] = 32767 * sin(2.0 * PI * frequency * i / sample_rate);
  }
}

bool beep(int frequency, int duration, float volume) {
  snd_pcm_t *pcm_handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_sw_params_t *sw_params;
  int dir;
  int sample_rate = 44100;
  int channels = 1;
  snd_pcm_uframes_t frames = 32;
  int16_t *buffer;

  // Open PCM device for playback
  if (snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
    fprintf(stderr, "Error opening PCM device %s\n", PCM_DEVICE);
    return false;
  }

  // Allocate a hardware parameters object
  snd_pcm_hw_params_malloc(&params);

  // Fill it in with default values
  snd_pcm_hw_params_any(pcm_handle, params);

  // Set the desired hardware parameters
  snd_pcm_hw_params_set_access(pcm_handle, params,
                               SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
  snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, &dir);
  snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);

  // Write the parameters to the driver
  if (snd_pcm_hw_params(pcm_handle, params) < 0) {
    fprintf(stderr, "Error setting HW params\n");
    snd_pcm_hw_params_free(params);
    snd_pcm_close(pcm_handle);
    return false;
  }

  // Allocate software parameters structure and initialize it
  snd_pcm_sw_params_malloc(&sw_params);
  snd_pcm_sw_params_current(pcm_handle, sw_params);
  snd_pcm_sw_params_set_avail_min(pcm_handle, sw_params, frames);
  snd_pcm_sw_params(pcm_handle, sw_params);

  // Use a buffer large enough to hold one period
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  buffer = (int16_t *)malloc(frames * channels * sizeof(int16_t));
  if (!buffer) {
    fprintf(stderr, "Error allocating buffer\n");
    snd_pcm_hw_params_free(params);
    snd_pcm_close(pcm_handle);
    return false;
  }

  // Generate a sine wave with adjusted volume
  int buffer_samples = frames * channels;
  int total_samples = sample_rate * duration / 1000;
  for (int i = 0; i < total_samples; i += buffer_samples) {
    generate_sine_wave(buffer, buffer_samples, frequency, sample_rate);

    // Adjust volume
    for (int j = 0; j < buffer_samples; j++) {
      buffer[j] = (int16_t)(buffer[j] * volume);
    }

    // Write to PCM device
    int err = snd_pcm_writei(pcm_handle, buffer, frames);
    if (err == -EPIPE) {
      fprintf(stderr, "XRUN.\n");
      snd_pcm_prepare(pcm_handle);
    } else if (err < 0) {
      fprintf(stderr, "Error writing to PCM device: %s\n", snd_strerror(err));
      free(buffer);
      snd_pcm_drain(pcm_handle);
      snd_pcm_close(pcm_handle);
      snd_pcm_hw_params_free(params);
      snd_pcm_sw_params_free(sw_params);
      return false;
    }
  }

  // Cleanup
  free(buffer);
  snd_pcm_drain(pcm_handle);
  snd_pcm_close(pcm_handle);
  snd_pcm_hw_params_free(params);
  snd_pcm_sw_params_free(sw_params);

  return true;
}
