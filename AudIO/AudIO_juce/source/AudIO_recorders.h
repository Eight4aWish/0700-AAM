// SampleRecorder and Looper, reconstructed.
//
// Kick uses both as engine objects: it hands them to DawEngine::createObject, routes them
// with initAudioRouting / connectInputTo, and reads their meters. AudIO already publishes
// the working part, AudIO::DRecorder (Daw/Objects/DawRecorder.h) over AudIO::AudioRecorder
// (Objects/Recorder.h), with the same seven callbacks as Kick's MTRecorderListener. The
// unpublished JUCE extension evidently wrapped it under the names Kick calls; these classes
// do the same and nothing more. Where AudioRecorder has no equivalent (the looper's undo and
// redo), the method is a documented no-op.

#pragma once

namespace mt
{
    namespace Recorder
    {
        using Direction = AudIO::AudioRecorder::Direction;
        using Mode      = AudIO::AudioRecorder::Mode;

        static constexpr Direction FORWARD{ AudIO::AudioRecorder::FORWARD };
        static constexpr Direction REVERSE{ AudIO::AudioRecorder::REVERSE };
    } // namespace Recorder

    // The JUCE-facing names Kick uses, mapped onto AudioRecorder.
    class JuceRecorder : public AudIO::DRecorder
    {
    public:
        explicit JuceRecorder( AudIO::Design& D ) : AudIO::DRecorder( D ) {}

        void setEnable( bool Enable ) { setBypass( !Enable ); }
        bool isEnabled() const { return !isBypassed(); }

        void  setMonitorOn( bool Monitor ) { setMonitor( Monitor ); }
        void  setGain( float Gain ) { setVolume( Gain ); }
        void  setGainDB( float GainDB ) { setVolumeDB( GainDB ); }
        float getGain() { return getVolume(); }
        void  setGainAndPan( float Gain, float Pan, bool /*ramp*/ ) { setVolumeAndPan( Gain, Pan ); }

        void         setRecordingMode( Recorder::Mode M ) { setRecordMode( M ); }
        Recorder::Mode getRecordingMode() const { return getRecordMode(); }
        bool         isForward() const { return isForwardDirection(); }

        // AudioRecorder keeps its take as one std::vector per channel; Kick wants a JUCE
        // buffer. This refers to that data rather than copying it.
        juce::AudioSampleBuffer& getAudioBuffer()
        {
            const auto& data = getRecorderData();
            channelPointers.clear();
            for( const auto& ch : data )
                channelPointers.push_back( const_cast<float*>( ch.data() ) );
            const int numSamples = data.empty() ? 0 : (int)data.front().size();
            if( channelPointers.empty() ) buffer.setSize( 0, 0 );
            else buffer.setDataToReferTo( channelPointers.data(), (int)channelPointers.size(), numSamples );
            return buffer;
        }

    private:
        juce::AudioSampleBuffer buffer;
        std::vector<float*>     channelPointers;
    };

    class SampleRecorder : public JuceRecorder
    {
    public:
        explicit SampleRecorder( AudIO::Design& D ) : JuceRecorder( D ) { setName( "SampleRecorder" ); }

        void startRecording( bool FromBeginning ) { start( getRecordMode(), getDirection(), FromBeginning ); }
        void stopRecording() { stop(); }
        void saveRecording( const juce::String& FilePath ) { save( FilePath.toStdString() ); }
    };

    class Looper : public JuceRecorder
    {
    public:
        explicit Looper( AudIO::Design& D ) : JuceRecorder( D ) { setName( "Looper" ); }

        using AudIO::AudioRecorder::start;
        void start( bool FromBeginning, bool /*SkipThresholdDetection*/ )
        {
            AudIO::AudioRecorder::start( getRecordMode(), getDirection(), FromBeginning );
        }
        void setRecord( bool Record )
        {
            if( Record ) start( false, false );
            else stop();
        }
        void setPlay() { startPreview( 0 ); }
        void play( double timeInSeconds ) { startPreview( timeInSeconds ); }
        void clear() { reset(); }
        void undo() {} // no equivalent in AudioRecorder
        void redo() {} // no equivalent in AudioRecorder
        void save( const juce::String& FilePath ) { AudIO::AudioRecorder::save( FilePath.toStdString() ); }

        void  setLength( double timeInSeconds )
        {
            length = (float)timeInSeconds;
            AudIO::AudioRecorder::setLength( timeInSeconds );
        }
        float getLength() const { return length; }
        void  setMonitorGain( float Gain ) { monitorGain = Gain; }
        float getMonitorGain() const { return monitorGain; }

        bool isEmpty()
        {
            const auto& data = getRecorderData();
            return data.empty() || data.front().empty();
        }
        bool    isRunning() const { return isRecording() || isPreviewing(); }
        double& getProgress()
        {
            progress = readProgress();
            return progress;
        }

    private:
        float  length{ 0.f };
        float  monitorGain{ 1.f };
        double progress{ 0.0 };
    };
} // namespace mt
