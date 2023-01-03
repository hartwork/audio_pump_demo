// Copyright (c) 2023 Sebastian Pipping <sebastian@pipping.org>
// Licensed under GPL v3 or later

#define _DEFAULT_SOURCE // for usleep

#include <pulse/mainloop-signal.h>
#include <pulse/mainloop.h>
#include <pulse/stream.h>
#include <pulse/thread-mainloop.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h> // for usleep

#define FREQUENCY 44100
#define CHANNELS 2

#define SAMPLE_CHANNEL_BITS 16
#define SAMPLE_FORMAT PA_SAMPLE_S16LE

#define SAMPLES 1024

void on_input_stream_data(pa_stream *p, size_t nbytes, void *userdata) {
  pa_stream *const output_stream = (pa_stream *)userdata;

  const void *buffer = NULL;
  const int peek_res = pa_stream_peek(p, &buffer, &nbytes);
  assert(peek_res == 0);

  const int drop_res = pa_stream_drop(p);
  assert(drop_res == 0);

  const int write_res =
      pa_stream_write(output_stream, buffer, nbytes, NULL, 0, PA_SEEK_RELATIVE);
  assert(write_res == 0);
}

void on_sigint(pa_mainloop_api *api, pa_signal_event *e, int sig,
               void *userdata) {
  (void)api;
  (void)e;
  (void)sig;
  pa_threaded_mainloop *const mainloop = (pa_threaded_mainloop *)userdata;
  pa_threaded_mainloop_signal(mainloop, 0);
}

int main() {
  const size_t buffer_size_bytes =
      SAMPLES * CHANNELS * (SAMPLE_CHANNEL_BITS / 8);
  const pa_sample_spec spec = {
      .format = SAMPLE_FORMAT, .rate = FREQUENCY, .channels = CHANNELS};

  // [1.1] mainloop new + API get
  pa_threaded_mainloop *const mainloop = pa_threaded_mainloop_new();
  assert(mainloop);
  pa_mainloop_api *const mainloop_api = pa_threaded_mainloop_get_api(mainloop);
  assert(mainloop_api);

  // [1.2] main loop signal init
  const int signal_init_ret = pa_signal_init(mainloop_api);
  assert(signal_init_ret == 0);

  // [1.3] main loop signal event new
  pa_signal_event *sigint_event = pa_signal_new(SIGINT, on_sigint, mainloop);
  assert(sigint_event);

  // [2.1] context new
  pa_context *const context = pa_context_new(mainloop_api, "async pump");
  assert(context);

  // [2.2] context connect
  const int connect_res =
      pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
  assert(connect_res == 0);

  // [1.5] mainloop start
  //       Starting the main loop prior to pa_context_connect would get us into
  //       this situation:
  //       Assertion 'c->callback' failed at
  //       ../pulseaudio-16.1/src/pulsecore/socket-client.c:126, function
  //       do_call(). Aborting.
  //       https://gitlab.freedesktop.org/pulseaudio/pulseaudio/-/issues/991
  const int mainloop_start_ret = pa_threaded_mainloop_start(mainloop);
  assert(mainloop_start_ret == 0);
  while (pa_context_get_state(context) < PA_CONTEXT_READY) {
    usleep(1000);
  }

  // [3.1] output stream new
  pa_stream *const output_stream =
      pa_stream_new(context, "playback", &spec, NULL);
  assert(output_stream);

  // [3.2] output stream connect
  const int output_connect_res =
      pa_stream_connect_playback(output_stream, NULL, NULL, 0, NULL, NULL);
  assert(output_connect_res == 0);

  // [4.1] input stream new + callback
  pa_stream *const input_stream = pa_stream_new(context, "record", &spec, NULL);
  assert(input_stream);
  pa_stream_set_read_callback(input_stream, on_input_stream_data,
                              output_stream);

  // [4.2] input stream connect
  while (pa_stream_get_state(output_stream) < PA_STREAM_READY) {
    usleep(1000);
  }
  const pa_buffer_attr input_buffer_attr = {
      .fragsize = buffer_size_bytes,
      .maxlength = buffer_size_bytes,
  };
  const int input_connect_res = pa_stream_connect_record(
      input_stream, NULL, &input_buffer_attr, PA_STREAM_ADJUST_LATENCY);
  assert(input_connect_res == 0);

  // Wait for signal
  fprintf(stderr, "Pumping from default input device to default output device, "
                  "press Ctrl+C to exit...");
  pa_threaded_mainloop_lock(mainloop);
  pa_threaded_mainloop_wait(mainloop);
  pa_threaded_mainloop_unlock(mainloop);

  // [4.2]
  pa_stream_disconnect(input_stream);

  // [4.1]
  pa_stream_unref(input_stream);

  // [3.2]
  pa_stream_disconnect(output_stream);

  // [3.1]
  pa_stream_unref(output_stream);

  // [2.2]
  pa_context_disconnect(context);

  // [2.1]
  pa_context_unref(context);

  // [1.3]
  pa_signal_free(sigint_event);

  // [1.2]
  pa_signal_done();

  // [1.5!]
  pa_threaded_mainloop_stop(mainloop);

  // [1.1]
  pa_threaded_mainloop_free(mainloop);

  return 0;
}
