[![Build on Linux](https://github.com/hartwork/audio_pump_demo/actions/workflows/linux.yml/badge.svg)](https://github.com/hartwork/audio_pump_demo/actions/workflows/linux.yml)
[![Enforce clang-format](https://github.com/hartwork/audio_pump_demo/actions/workflows/clang-format.yml/badge.svg)](https://github.com/hartwork/audio_pump_demo/actions/workflows/clang-format.yml)


# Audio pump demo (for PulseAudio, PortAudio and SDL 2)

This repository demos pumping audio
from the default input device (e.g. a microphone)
to the default output device (e.g. headphones/speakers)
in C99.
In other words: this demo records audio, and it plays back audio.

There are four editions:

- one for [PortAudio](https://github.com/PortAudio/portaudio) using asynchronous callbacks —
  [`portaudio_async_pump_demo.c`](https://github.com/hartwork/audio_pump_demo/blob/master/portaudio_async_pump_demo.c),
- one for [SDL 2](https://wiki.libsdl.org/SDL2/FrontPage) —
  [`sdl2_audio_pump_demo.c`](https://github.com/hartwork/audio_pump_demo/blob/master/sdl2_audio_pump_demo.c),
- one for the [Simple API](https://freedesktop.org/software/pulseaudio/doxygen/simple.html) of PulseAudio —
  [`pulseaudio_simple_pump_demo.c`](https://github.com/hartwork/audio_pump_demo/blob/master/pulseaudio_simple_pump_demo.c),
- one for the [Asynchronous API](https://freedesktop.org/software/pulseaudio/doxygen/async.html) of PulseAudio —
  [`pulseaudio_async_pump_demo.c`](https://github.com/hartwork/audio_pump_demo/blob/master/pulseaudio_async_pump_demo.c).

The code is Software Libre licensed under GPL v3 or later.


# Requirements

- libpulse
- libpulse-simple
- PortAudio
- SDL 2.x

e.g.:

```console
# sudo apt install --no-install-recommends --yes -V \
    build-essential \
    libpulse-dev \
    libsdl2-dev \
    portaudio19-dev
```


# Compilation

```console
# make CFLAGS='-O2 -pipe' -j
# ./portaudio_async_pump_demo
# ./pulseaudio_async_pump_demo
# ./pulseaudio_simple_pump_demo
# ./sdl2_audio_pump_demo
```


# Hotkeys

- <kbd>Ctrl</kbd>+<kbd>C</kbd> — exit
