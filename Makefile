# Copyright (c) 2023 Sebastian Pipping <sebastian@pipping.org>
# Licensed under GPL v3 or later

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
INSTALL = install

CFLAGS += -Wall -Wextra -std=c99 -pedantic

PORTAUDIO_CFLAGS := $(shell pkg-config --cflags portaudio-2.0)
PORTAUDIO_LDFLAGS := $(shell pkg-config --libs portaudio-2.0)

PULSE_CFLAGS := $(shell pkg-config --cflags libpulse)
PULSE_LDFLAGS := $(shell pkg-config --libs libpulse)

PULSE_SIMPLE_CFLAGS := $(shell pkg-config --cflags libpulse-simple)
PULSE_SIMPLE_LDFLAGS := $(shell pkg-config --libs libpulse-simple)

SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL2_LDFLAGS := $(shell pkg-config --libs sdl2)

ALL_APPS = \
	portaudio_async_pump_demo \
	pulseaudio_async_pump_demo \
	pulseaudio_simple_pump_demo \
	sdl2_audio_pump_demo

.PHONY: all
all: $(ALL_APPS)

.PHONY: clean
clean:
	$(RM) $(ALL_APPS)

portaudio_async_pump_demo: portaudio_async_pump_demo.c
	$(CC) $< -o $@ $(PORTAUDIO_CFLAGS) $(CFLAGS) $(PORTAUDIO_LDFLAGS) $(LDFLAGS)

pulseaudio_async_pump_demo: pulseaudio_async_pump_demo.c
	$(CC) $< -o $@ $(PULSE_CFLAGS) $(CFLAGS) $(PULSE_LDFLAGS) $(LDFLAGS)

pulseaudio_simple_pump_demo: pulseaudio_simple_pump_demo.c
	$(CC) $< -o $@ $(PULSE_SIMPLE_CFLAGS) $(CFLAGS) $(PULSE_SIMPLE_LDFLAGS) $(LDFLAGS)

sdl2_audio_pump_demo: sdl2_audio_pump_demo.c
	$(CC) $< -o $@ $(SDL2_CFLAGS) $(CFLAGS) $(SDL2_LDFLAGS) $(LDFLAGS)

.PHONY: install
install: $(ALL_APPS)
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 0755 -t $(DESTDIR)$(BINDIR) $(ALL_APPS)
