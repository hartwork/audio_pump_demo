// Copyright (c) 2023 Sebastian Pipping <sebastian@pipping.org>
// Licensed under GPL v3 or later

#include <portaudio.h>

#include <signal.h>
#include <sys/signalfd.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // for NULL
#include <string.h> // memset
#include <unistd.h> // for read

#define FREQUENCY 44100
#define CHANNELS 2

#define SAMPLE_CHANNEL_BITS 16
#define SAMPLE_FORMAT paInt16

#define FRAMES 512

int on_input_stream_data(const void *input, void *output,
                         unsigned long frameCount,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  (void)timeInfo;
  (void)statusFlags;
  (void)userData;

  const size_t bytes_to_write =
      (SAMPLE_CHANNEL_BITS / 8) * frameCount * CHANNELS;

  assert(output != NULL);

  if (input == NULL) {
    memset(output, 0, bytes_to_write); // i.e. silence
  } else {
    memcpy(output, input, bytes_to_write);
  }

  return paContinue;
}

int main() {
  // [1.1] init
  const PaError init_error = Pa_Initialize();
  assert(init_error == paNoError);

  // [2.1] open duplex stream
  const PaDeviceIndex input_device = Pa_GetDefaultInputDevice();
  assert(input_device != paNoDevice);
  const PaDeviceIndex output_device = Pa_GetDefaultOutputDevice();
  assert(output_device != paNoDevice);
  PaStream *duplex_stream = NULL;
  const double latency_seconds = 1.0 / FREQUENCY * FRAMES;
  const PaStreamParameters input_parameters = {
      .device = input_device,
      .channelCount = CHANNELS,
      .sampleFormat = SAMPLE_FORMAT,
      .suggestedLatency = latency_seconds,
      .hostApiSpecificStreamInfo = NULL};
  const PaStreamParameters output_parameters = {
      .device = output_device,
      .channelCount = CHANNELS,
      .sampleFormat = SAMPLE_FORMAT,
      .suggestedLatency = latency_seconds,
      .hostApiSpecificStreamInfo = NULL};
  const PaError open_error =
      Pa_OpenStream(&duplex_stream, &input_parameters, &output_parameters,
                    FREQUENCY, FRAMES, paClipOff, on_input_stream_data, NULL);
  assert(open_error == paNoError);
  assert(duplex_stream != NULL);

  // [2.2] start stream
  const PaError input_start_error = Pa_StartStream(duplex_stream);
  assert(input_start_error == paNoError);

  fprintf(stderr, "Pumping from default input device to default output device, "
                  "press Ctrl+C to exit...");

  sigset_t signal_mask;
  sigemptyset(&signal_mask);
  sigaddset(&signal_mask, SIGINT);
  const int signals_fd = signalfd(-1, &signal_mask, SFD_NONBLOCK);
  assert(signals_fd >= 0);
  const int sigproc_ret = sigprocmask(SIG_BLOCK, &signal_mask, NULL);
  assert(sigproc_ret != -1);

  for (;;) {
    struct signalfd_siginfo signal_info;
    ssize_t signal_bytes_read =
        read(signals_fd, &signal_info, sizeof(signal_info));
    if (signal_bytes_read == sizeof(signal_info)) {
      break;
    }
  }

  close(signals_fd);

  // [2.2]
  const PaError stop_error = Pa_StopStream(duplex_stream);
  assert(stop_error == paNoError);

  // [2.1]
  const PaError close_error = Pa_CloseStream(duplex_stream);
  assert(close_error == paNoError);

  // [1.1]
  const PaError terminate_error = Pa_Terminate();
  assert(terminate_error == paNoError);

  return 0;
}
