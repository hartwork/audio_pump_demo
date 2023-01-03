// Copyright (c) 2023 Sebastian Pipping <sebastian@pipping.org>
// Licensed under GPL v3 or later

#include <pulse/simple.h>

#include <sys/signalfd.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // for malloc
#include <unistd.h> // for read

#define FREQUENCY 44100
#define CHANNELS 2

#define SAMPLE_CHANNEL_BITS 16
#define SAMPLE_FORMAT PA_SAMPLE_S16LE

#define SAMPLES 1024

int main() {
  const size_t buffer_size_bytes =
      SAMPLES * CHANNELS * (SAMPLE_CHANNEL_BITS / 8);

  const pa_sample_spec spec = {
      .format = SAMPLE_FORMAT, .rate = FREQUENCY, .channels = CHANNELS};

  const pa_buffer_attr input_buffer_attr = {
      .fragsize = buffer_size_bytes,
      .maxlength = buffer_size_bytes,
  };

  int input_error = 0;
  pa_simple *const input =
      pa_simple_new(NULL, "simple pump", PA_STREAM_RECORD, NULL, "record",
                    &spec, NULL, &input_buffer_attr, &input_error);
  assert(input != NULL);

  int output_error = 0;
  pa_simple *const output =
      pa_simple_new(NULL, "simple pump", PA_STREAM_PLAYBACK, NULL, "playback",
                    &spec, NULL, NULL, &output_error);
  assert(output != NULL);

  void *const buffer = malloc(buffer_size_bytes);
  assert(buffer != NULL);

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
    int read_error = 0;
    const int read_ret =
        pa_simple_read(input, buffer, buffer_size_bytes, &read_error);
    assert(read_ret >= 0);

    int write_error = 0;
    const int write_ret =
        pa_simple_write(output, buffer, buffer_size_bytes, &write_error);
    assert(write_ret >= 0);

    struct signalfd_siginfo signal_info;
    ssize_t signal_bytes_read =
        read(signals_fd, &signal_info, sizeof(signal_info));
    if (signal_bytes_read == sizeof(signal_info)) {
      break;
    }
  }

  close(signals_fd);

  free(buffer);

  pa_simple_free(output);

  pa_simple_free(input);

  return 0;
}
