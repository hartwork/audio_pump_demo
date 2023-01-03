[![Build on Linux](https://github.com/hartwork/audio_pump_demo/actions/workflows/linux.yml/badge.svg)](https://github.com/hartwork/audio_pump_demo/actions/workflows/linux.yml)
[![Enforce clang-format](https://github.com/hartwork/audio_pump_demo/actions/workflows/clang-format.yml/badge.svg)](https://github.com/hartwork/audio_pump_demo/actions/workflows/clang-format.yml)


# Audio pump demo (for both PulseAudio and SDL 2)

This repository demos pumping audio from
the default input device to the default output device in C99.

There are three versions:

- one for [SDL 2](https://wiki.libsdl.org/SDL2/FrontPage),
- one for the [Simple API](https://freedesktop.org/software/pulseaudio/doxygen/simple.html) of PulseAudio,
- one for the [Asynchronous API](https://freedesktop.org/software/pulseaudio/doxygen/async.html) of PulseAudio.

The code is Software Libre licensed under GPL v3 or later.


# Requirements

- libpulse
- libpulse-simple
- SDL 2.x

e.g.:

```console
# sudo apt install --no-install-recommends --yes -V \
    build-essential \
    libpulse-dev \
    libsdl2-dev
```


# Compilation

```console
# make CFLAGS='-O2 -pipe' -j
# ./pulseaudio_async_pump_demo
# ./pulseaudio_simple_pump_demo
# ./sdl2_audio_pump_demo
```


# Hotkeys

- <kbd>Ctrl</kbd>+<kbd>C</kbd> — exit
