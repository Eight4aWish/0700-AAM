/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 AudIO
  vendor:             MUSIC Tribe, MY
  version:            0.1.0
  name:               MusicTribe AudIO definitions
  description:        Classes and interfaces containing AudIO definitions
  website:
  license:            AGPLv3

  dependencies:       

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

/*
 * Copyright (c) 2022 MUSIC Tribe
 *
 * This file is part of AudIO.
 *
 * AudIO is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * AudIO is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with AudIO. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

// AUDIO Configuration
//#define AUDIO_ONLY_CORE                  ( 0 )
#define AUDIO_DAW_CORE                   ( 1 )
#define AUDIO_JUCE_EXTENSIONS            ( 1 )

#define AUDIO_DAW_EXTENSIONS             ( 0 )

#define AUDIO_OBJECT_AUDIO_READER_WRITER ( 0 )
#define AUDIO_OBJECT_AUDIO_FILE_PLAYBACK ( 0 )
#define AUDIO_OBJECT_AUDIO_FILE_RECORDER ( 1 ) // Kick needs it: its SampleRecorder and Looper are built on DRecorder
#define AUDIO_OBJECT_SF_SYNTH            ( 0 )

#pragma once
#ifndef AUDIO_H_INCLUDE
    #define AUDIO_H_INCLUDE

    // Dependent module includes...

    // Module include files...
    #include "Includes/Utils.h"
    #include "Includes/Platform.h"
    #include "Includes/AudioDataTypes.h"
    #include "Includes/RampedParameter.h"

    // DSP Constructs....
    #include "Dsp/adsr.h"
    #include "Dsp/filter.h"
    #include "Dsp/lfo.h"
    #include "Dsp/pan.h"

    // Core files...
    #include "Includes/AudIOBuffer.h"
    #include "Includes/ObjectFactory.h"

    #include "Includes/IProcessable.h"
    #include "Includes/IAudioDevice.h"
    #include "Includes/AudioObject.h"
    #include "Includes/DeviceManager.h"

// Devices...

    #if !defined( AUDIO_ONLY_CORE )
        // DSP Processing Objects...
        #include "Objects/ADSR.h"
        #include "Objects/LevelMeter.h"
        #include "Objects/Mixer.h"
        #include "Objects/SineWave.h"
        #include "Objects/VolumeAndPan.h"
        #include "Objects/WhiteNoise.h"

        #if defined( AUDIO_OBJECT_AUDIO_READER_WRITER ) && ( AUDIO_OBJECT_AUDIO_READER_WRITER > 0 )
            #include "Objects/AudioReaderWriter.h"
        #endif

        #if defined( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK ) && ( AUDIO_OBJECT_AUDIO_FILE_PLAYBACK > 0 )
            #include "Objects/PlayBack.h"
        #endif

        #if defined( AUDIO_OBJECT_AUDIO_FILE_RECORDER ) && ( AUDIO_OBJECT_AUDIO_FILE_RECORDER > 0 )
            #include "Objects/Recorder.h"
        #endif

        #if defined( AUDIO_OBJECT_SF_SYNTH ) && ( AUDIO_OBJECT_SF_SYNTH > 0 )
            #include "Objects/SFSynth.h"
        #endif

        #if defined( AUDIO_OBJECT_AUDIO_READER_WRITER ) && ( AUDIO_OBJECT_AUDIO_READER_WRITER > 0 )
        #endif

        #if defined( AUDIO_DAW_CORE ) && ( AUDIO_DAW_CORE > 0 )
            // DAW Framework
            #include "Daw/DawTypes.h"
            #include "Daw/DawTime.h"
            #include "Daw/DawBus.h"
            #include "Daw/DawEngine.h"
            #include "Daw/DawInserts.h"

            #if defined( AUDIO_DAW_EXTENSIONS ) && ( AUDIO_DAW_EXTENSIONS > 0 )
                #include "Daw/DawTrack.h"
                #include "Daw/DawSequence.h"
            #endif
        #endif // AUDIO_DAW_CORE

        #if defined( AUDIO_JUCE_EXTENSIONS ) && ( AUDIO_JUCE_EXTENSIONS > 0 )
        #endif // AUDIO_JUCE_EXTENSIONS

    #endif

#endif // AUDIO_H_INCLUDE
