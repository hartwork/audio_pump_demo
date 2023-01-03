// Copyright (c) 2023 Sebastian Pipping <sebastian@pipping.org>
// Licensed under GPL v3 or later

#include <SDL2/SDL.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define FREQUENCY 44100
#define CHANNELS 2

#define SAMPLE_CHANNEL_BITS 16
#define SAMPLE_FORMAT AUDIO_S16SYS

#define SAMPLES 1024

void input_callback(void *userdata, Uint8 *stream, int len /*in bytes*/) {
  if (len < 1) {
    return;
  }

  const SDL_AudioDeviceID output_device = (SDL_AudioDeviceID)(intptr_t)userdata;
  assert(output_device != 0);

  const int queued_ret = SDL_QueueAudio(output_device, stream, len);
  assert(queued_ret == 0);
}

int main() {
  const int init_ret = SDL_Init(SDL_INIT_AUDIO);
  assert(init_ret == 0);

  const SDL_AudioSpec output_spec_wanted = {
      .freq = FREQUENCY,
      .format = SAMPLE_FORMAT,
      .channels = CHANNELS,
      .samples = SAMPLES,
      .callback = NULL,
  };

  SDL_AudioDeviceID output_device =
      SDL_OpenAudioDevice(NULL, 0, &output_spec_wanted, NULL, 0);
  assert(output_device != 0);

  const SDL_AudioSpec input_spec_wanted = {
      .freq = FREQUENCY,
      .format = SAMPLE_FORMAT,
      .channels = CHANNELS,
      .samples = SAMPLES,
      .callback = input_callback,
      .userdata = (void *)(intptr_t)output_device,
  };

  SDL_AudioDeviceID input_device =
      SDL_OpenAudioDevice(NULL, 1, &input_spec_wanted, NULL, 0);
  assert(input_device != 0);

  fprintf(stderr, "Pumping from default input device to default output device, "
                  "press Ctrl+C to exit...\n");

  SDL_PauseAudioDevice(output_device, 0);
  SDL_PauseAudioDevice(input_device, 0);

  SDL_Event event;
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type)
      case SDL_QUIT:
        running = false;
      break;
    }
    SDL_Delay(1);
  }

  SDL_CloseAudioDevice(output_device);
  SDL_CloseAudioDevice(input_device);

  return 0;
}
