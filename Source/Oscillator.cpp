//
// Created by alisdair chauvin on 12/2/25.
//

#include "Oscillator.h"

Oscillator::Oscillator() {}

void Oscillator::prepare(double sampleRate, int samplesPerBlock, int channels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = channels;

    osc.prepare(spec);
    gain.prepare(spec);
    osc.setFrequency(440.0f);
    gain.setGainLinear(0.2f);
}

void Oscillator::process (juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> audioBlock (buffer);
    juce::dsp::ProcessContextReplacing<float> context (audioBlock);

    osc.process (context);
    gain.process (context);
}

void Oscillator::setFrequency (float freq) {
    osc.setFrequency(freq);
}

void Oscillator::setGain (float newGain)
{
    gain.setGainLinear (newGain);
}

void Oscillator::setWaveform (int type)
{
    initWaveform (type);
}

void Oscillator::initWaveform (int waveformIndex)
{
    switch (waveformIndex)
    {
        case 0: // Sine
            osc.initialise ([] (float x) { return std::sin (x); });
            break;

        case 1: // Square
            osc.initialise ([] (float x)
            {
                return x < 0.0f ? -1.0f : 1.0f;
            });
            break;

        case 2: // Saw
            osc.initialise ([] (float x)
            {
                return juce::jmap (x,
                    -juce::MathConstants<float>::pi,
                     juce::MathConstants<float>::pi,
                    -1.0f, 1.0f);
            });
            break;

        case 3: // Triangle
            osc.initialise ([] (float x)
            {
                return asinf (std::sin (x)) * (2.0f / juce::MathConstants<float>::pi);
            });
            break;

        case 4: // Noise
            osc.initialise ([] (float)
            {
                return juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
            }, 256); // table size
            break;

        case 5: // Additive 1 (sine + 2nd harmonic)
            osc.initialise ([] (float x)
            {
                return std::sin(x) + 0.3f * std::sin(2.0f * x);
            });
            break;

        case 6: // Additive 2 (sine + 2nd + 3rd)
            osc.initialise ([] (float x)
            {
                return std::sin(x)
                     + 0.3f * std::sin(2.0f * x)
                     + 0.15f * std::sin(3.0f * x);
            });
            break;

        default:
            osc.initialise ([] (float x) { return std::sin (x); });
            break;
    }
}
