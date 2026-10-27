# Building the KICK app on a Mac

This is the `macos-build` branch of [Eight4aWish/0700-AAM](https://github.com/Eight4aWish/0700-AAM),
a fork of [MusicTribeOpenSource/0700-AAM](https://github.com/MusicTribeOpenSource/0700-AAM). It is
not made, approved or supported by Music Tribe. `main` follows upstream unchanged.

The release does not build as published (issue #9): two modules the app includes, and the
CMake file that registers them, were not released; a third module was removed for licensing;
and AudIO ships with its recorder switched off. This branch fills those gaps so that the KICK's own app (`Kick`
target) builds and runs on macOS. It starts and shows its Projects screen at 1024 × 600.

Built on macOS (Apple Silicon) with Apple clang 21, CMake 4.0 and Ninja, against JUCE 7.0.12
with the release's own multitouch patch.

```bash
Kick/build-mac.sh          # configure + build the Kick target
open Kick/build-sim/Kick.app
```

## What was missing, and what stands in for it

Everything below is written from how Kick uses each piece: the names, types and call sites
in the released code. None of it is Music Tribe's original, and where a value is a guess
the header says so.

| Missing piece | Stand-in | Notes |
| --- | --- | --- |
| `AudIO/juce_modules.cmake` | registers `AudIO`, `AudIO_constants`, `AudIO_juce` | three lines |
| `AudIO_constants` module | `MIDI`, `MIDI_CH`, `POLYPHONY`, `GAIN`, `PAN`, `DB` ranges, `UNKNOWN_VALUE`, the engine's sample rate, block size and bus counts | ranges follow `kick_Constants.h`; guesses are marked |
| `AudIO_juce` module | includes AudIO and the JUCE audio modules, and supplies the classes below | the `AUDIO_JUCE_EXTENSIONS` block in `AudIO.h` is empty in the release |
| `SampleRecorder`, `Looper` | thin subclasses of AudIO's published `DRecorder` / `AudioRecorder`, under the names Kick calls | looper undo/redo are no-ops |
| `mt::AudioDevice` | AudIO's `IAudioDevice` driven by `juce::AudioDeviceManager` | |
| `mt::Filter`, `FilterType` | a TPT state-variable filter covering the list in `kick_Constants.h` | pole counts approximated; "Model" types fall back to low-pass |
| `mt::ADSR` five-argument `setParameters` | a subclass of AudIO's `ADSR<float>` | reorders to AudIO's `Params` |
| `juce_serialport` (removed: no licence) | a POSIX termios serial port with JUCE streams | the panel link on the device |

Smaller changes: AudIO's `AUDIO_OBJECT_AUDIO_FILE_RECORDER` switched on (Kick's recorder
needs it); `<thread>` and `<sys/stat.h>` included where libc++ and macOS need them; the
simulator target generates `config.h` like the other apps.

## Not done yet

- **The simulator (`KT_KickEM`)** still fails: its panel code predates the current packet
  protocol (it uses `sig::Signal`, `mt::Kick::HwControlCommands` and button helpers that
  no longer exist). The `Kick` app does not need it.
- **Quitting** ends in an abort (exit 134) after the window closes.
- Audio, recording and the drum filter compile and are wired in, but have not been listened to.
- Nothing has been built for the device itself (aarch64 Buildroot).

`Kick/apps/kick/Main.cpp` also gains a desktop-only debugging aid: set `KICK_SNAPSHOT` to a
PNG path and the app saves its window there after a few seconds and quits.
