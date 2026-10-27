/*******************************************************************************
 Reconstruction of a module that Kick includes but that was not in this release.
 Built up from the names and types Kick uses, so the simulator compiles; it is not
 Music Tribe's original.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 AudIO_constants
  vendor:             Eight4aWish (reconstruction)
  version:            0.0.1
  name:               AudIO_constants (reconstructed)
  description:        Stand-in for the unreleased AudIO_constants module
  website:            https://github.com/Eight4aWish
  license:            AGPLv3

  dependencies:       juce_core

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

// Ranges in the same shape as Kick/modules/common/source/kick_Constants.h (BPM, BAR, SEMI...).
// Each value is inferred from how Kick uses it; "guess" marks the ones the code does not pin down.

// A sentinel for "not attached yet" (kick_attachments.h wraps it in a juce::var).
static constexpr int UNKNOWN_VALUE{ -1 };

namespace MIDI
{
    static constexpr int   MIN{ 0 };
    static constexpr int   MAX{ 127 };
    static constexpr int   STEP{ 1 };
    static constexpr int   NUMERIC_MAX{ 128 };       // sizes the per-note pad-colour arrays
    static constexpr int   MIDDLE_C{ 60 };           // keygroup key note
    static constexpr int   NOTE_DEFAULT{ 60 };       // guess
    static constexpr int   DEFAULT{ 100 };           // guess (MPC-style default velocity)
    static constexpr int   LEVEL_DEFAULT{ 100 };     // guess; also the top of the Q-Link level range
    static constexpr float SCALER{ 1.0f / 127.0f };  // 0..127 -> 0..1 (metronome gain, meters)
}

namespace MIDI_CH
{
    static constexpr int MIN{ 1 };
    static constexpr int MAX{ 16 };
    static constexpr int DEFAULT{ 1 };
}

namespace POLYPHONY
{
    static constexpr int MIN{ 1 };
    static constexpr int MAX{ 32 };       // guess; ISynthObject allocates this many voices
    static constexpr int STEP{ 1 };
    static constexpr int DEFAULT{ 16 };   // guess
}

// Linear gain: the code uses ROOT_2 (+3 dB) as a default and 0.01 steps.
namespace GAIN
{
    static constexpr float MIN{ 0.0f };
    static constexpr float MAX{ 2.0f };   // guess (+6 dB)
    static constexpr float STEP{ 0.01f };
    static constexpr float DEFAULT{ 1.0f };
    static constexpr float ROOT_2{ 1.41421356f };
}

namespace PAN
{
    static constexpr float MIN{ -1.0f };  // guess: the UI draws L and R either side
    static constexpr float MAX{ 1.0f };
    static constexpr float STEP{ 0.01f };
    static constexpr float DEFAULT{ 0.0f };
}

namespace DB
{
    static constexpr float MIN{ -96.0f };  // guess: the meter floor for a gain of zero
}

// The audio engine's shape: DawEngine( ins, outs, send/returns, submixes, auxes ).
static constexpr double       AUDIO_SAMPLE_RATE{ 48000.0 };  // asound.conf on the device runs at 48 kHz
static constexpr int          AUDIO_BLOCK_SIZE{ 256 };       // guess
static constexpr unsigned int AUDIO_NUM_INPUTS{ 2 };         // the V4220M codec: stereo in
static constexpr unsigned int AUDIO_NUM_OUTPUTS{ 2 };        // and stereo out
static constexpr unsigned int AUDIO_NUM_SEND_RETURNS{ 4 };   // guess
static constexpr unsigned int AUDIO_NUM_SUBMIX{ 8 };         // guess (MPC-style eight submixes)
static constexpr unsigned int AUDIO_NUM_AUX{ 2 };            // guess
