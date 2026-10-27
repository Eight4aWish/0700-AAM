// mt::Filter and mt::FilterType, reconstructed for the drum voice.
//
// DrumVoice selects a type by index into kick_Constants' filterTypes list ("Lowpass 1 Pole"
// ... "Kick3000 LPF"), sets resonance on a 0-100 scale (default 10), sets a cutoff
// normalised through Filter::freqRange (Hz, default 20000) and filters a small buffer in
// place. The original is not in the release. This is a TPT state-variable filter: the
// families (low, high, band pass, band stop, band boost) are right, the pole counts are
// approximated by cascading two-pole sections, and the four "Model" types and Kick3000 LPF
// fall back to a two-pole low-pass.

#pragma once

namespace mt
{
    enum FilterType
    {
        Lowpass1, Lowpass2, Lowpass4, Lowpass6, Lowpass8,
        Highpass1, Highpass2, Highpass4, Highpass6, Highpass8,
        Bandpass2, Bandpass4, Bandpass6, Bandpass8,
        Bandstop2, Bandstop4, Bandstop6, Bandstop8,
        Bandboost2, Bandboost4, Bandboost6, Bandboost8,
        Model1, Model2, Model3, Model4,
        Kick3000LPF
    };

    class Filter
    {
    public:
        static inline const juce::NormalisableRange<float> freqRange = [] {
            juce::NormalisableRange<float> r( 20.f, 20000.f );
            r.setSkewForCentre( 1000.f );
            return r;
        }();

        void setSampleRate( double sr ) { sampleRate = sr > 0 ? sr : 48000.0; update(); }
        void reset() { for( auto& s : state ) s = {}; }
        void setFilterType( FilterType t )
        {
            type = t;
            update();
        }
        void setResonance( float r ) { resonance = juce::jlimit( 0.f, 100.f, r ); update(); }
        void setCutoff( float normalised )
        {
            cutoffHz = freqRange.convertFrom0to1( juce::jlimit( 0.f, 1.f, normalised ) );
            update();
        }

        void process( juce::AudioBuffer<float>& buffer )
        {
            const int chans = juce::jmin( buffer.getNumChannels(), (int)state.size() );
            for( int ch = 0; ch < chans; ++ch )
            {
                auto* d = buffer.getWritePointer( ch );
                for( int i = 0; i < buffer.getNumSamples(); ++i )
                    for( int s = 0; s < stages; ++s )
                        d[i] = tick( state[(size_t)ch][(size_t)s], d[i] );
            }
        }

    private:
        enum class Mode { low, high, band, stop, boost };
        struct Section { float ic1 = 0, ic2 = 0; };

        float tick( Section& z, float x ) const
        {
            const float v3 = x - z.ic2;
            const float v1 = a1 * z.ic1 + a2 * v3;
            const float v2 = z.ic2 + a2 * z.ic1 + a3 * v3;
            z.ic1          = 2 * v1 - z.ic1;
            z.ic2          = 2 * v2 - z.ic2;
            switch( mode )
            {
                case Mode::low: return v2;
                case Mode::high: return x - k * v1 - v2;
                case Mode::band: return v1;
                case Mode::stop: return x - k * v1;
                case Mode::boost: return x + k * v1;
            }
            return v2;
        }

        void update()
        {
            const int t = (int)type;
            if( t <= Lowpass8 ) { mode = Mode::low; stages = juce::jmax( 1, ( t - Lowpass1 + 1 ) / 2 ); }
            else if( t <= Highpass8 ) { mode = Mode::high; stages = juce::jmax( 1, ( t - Highpass1 + 1 ) / 2 ); }
            else if( t <= Bandpass8 ) { mode = Mode::band; stages = t - Bandpass2 + 1; }
            else if( t <= Bandstop8 ) { mode = Mode::stop; stages = t - Bandstop2 + 1; }
            else if( t <= Bandboost8 ) { mode = Mode::boost; stages = 1; }
            else { mode = Mode::low; stages = 1; }

            const double fc = juce::jlimit( 20.0, sampleRate * 0.45, (double)cutoffHz );
            const float  g  = (float)std::tan( juce::MathConstants<double>::pi * fc / sampleRate );
            k               = 2.f - 1.9f * ( resonance / 100.f ); // Q from about 0.5 to 10
            a1              = 1.f / ( 1.f + g * ( g + k ) );
            a2              = g * a1;
            a3              = g * a2;
        }

        double     sampleRate{ 48000.0 };
        FilterType type{ Lowpass2 };
        Mode       mode{ Mode::low };
        int        stages{ 1 };
        float      cutoffHz{ 20000.f }, resonance{ 10.f };
        float      k{ 1.4f }, a1{ 0 }, a2{ 0 }, a3{ 0 };
        std::array<std::array<Section, 4>, 2> state{};
    };
} // namespace mt
