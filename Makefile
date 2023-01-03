# Copyright (c) 2023 Sebastian Pipping <sebastian@pipping.org>
# Licensed under GPL v3 or later

CFLAGS += -Wall -Wextra -std=c99 -pedantic

PULSE_CFLAGS := $(shell pkg-config --cflags libpulse)
PULSE_LDFLAGS := $(shell pkg-config --libs libpulse)

PULSE_SIMPLE_CFLAGS := $(shell pkg-config --cflags libpulse-simple)
PULSE_SIMPLE_LDFLAGS := $(shell pkg-config --libs libpulse-simple)

SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL2_LDFLAGS := $(shell pkg-config --libs sdl2)

ALL_APPS = \
	pulseaudio_async_pump_demo \
	pulseaudio_simple_pump_demo \
	sdl2_audio_pump_demo

.PHONY: all
all: $(ALL_APPS)

.PHONY: clean
clean:
	$(RM) $(ALL_APPS)

pulseaudio_async_pump_demo: pulseaudio_async_pump_demo.c
	$(CC) $< -o $@ $(PULSE_CFLAGS) $(CFLAGS) $(PULSE_LDFLAGS) $(LDFLAGS)

pulseaudio_simple_pump_demo: pulseaudio_simple_pump_demo.c
	$(CC) $< -o $@ $(PULSE_SIMPLE_CFLAGS) $(CFLAGS) $(PULSE_SIMPLE_LDFLAGS) $(LDFLAGS)

sdl2_audio_pump_demo: sdl2_audio_pump_demo.c
	$(CC) $< -o $@ $(SDL2_CFLAGS) $(CFLAGS) $(SDL2_LDFLAGS) $(LDFLAGS)
