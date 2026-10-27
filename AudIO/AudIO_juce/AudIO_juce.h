/*******************************************************************************
 Reconstruction of a module that Kick includes but that was not in this release.
 Built up from the names and types Kick uses, so the simulator compiles; it is not
 Music Tribe's original.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 AudIO_juce
  vendor:             Eight4aWish (reconstruction)
  version:            0.0.1
  name:               AudIO_juce (reconstructed)
  description:        Stand-in for the unreleased AudIO_juce module
  website:            https://github.com/Eight4aWish
  license:            AGPLv3

  dependencies:       AudIO, AudIO_constants, juce_audio_basics, juce_audio_devices, juce_audio_utils

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#pragma once

// The JUCE glue around AudIO: Kick reaches the whole engine through this module.
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <AudIO_constants/AudIO_constants.h>
// Recorder.h only includes its WAV writer when JUCE is absent; under JUCE the original
// extension supplied it. The bundled header-only AudioFile works either way.
#include <AudIO/3rdparty/AudioFile.h>
#include <AudIO/AudIO.h>

// Kick's drum voice declares bare ADSR members and calls gate() on them: AudIO's own envelope,
// not juce::ADSR. Inside namespace mt this wins over any using-directive. Its Filter is a
// separate reconstruction (source/AudIO_filter.h).
namespace mt
{
    // DrumVoice calls setParameters( attack, hold, decay, release, sustain ), already in
    // samples; AudIO's ADSR takes a Params struct ordered A, H, D, S, R.
    class ADSR : public AudIO::ADSR<float>
    {
    public:
        using AudIO::ADSR<float>::ADSR;
        using AudIO::ADSR<float>::setParameters;
        void setParameters( double A, double H, double D, double R, double S ) noexcept
        {
            setParameters( Params{ A, H, D, S, R } );
        }
    };
}

// kick_Constants.h opens namespace EMPTY with an empty ValueTree; Kick also expects an empty
// sample buffer there (audio_sound.cpp returns it when a slot has no source).
namespace EMPTY
{
    static const juce::AudioBuffer<float> SampleBuffer{};
    static const juce::String             String{};
}

#include "source/AudIO_recorders.h"
#include "source/AudIO_filter.h"
#include "source/AudIO_device.h"
