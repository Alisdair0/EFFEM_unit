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
    switch (type)
    {
        case 0: osc.initialise([](float x){ return std::sin(x); }); break;
        case 1: osc.initialise([](float x){ return x < 0.0f ? -1.0f : 1.0f; }); break; // square
        case 2: osc.initialise([](float x){ return (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(x)); }); break; // triangle
        case 3: osc.initialise([](float x){ return x / juce::MathConstants<float>::pi; }); break; // saw
        default: osc.initialise([](float x){ return std::sin(x); }); break;
    }
}

void Oscillator::initWaveform (int waveformIndex)
{
    switch (waveformIndex)
    {
        case Sine:
            osc.initialise ([] (float x)
            {
                return std::sin (x);
            });
            break;

        case Saw:
            osc.initialise ([] (float x)
            {
                return juce::jmap (x,
                    -juce::MathConstants<float>::pi,
                     juce::MathConstants<float>::pi,
                    -1.0f, 1.0f);
            });
            break;

        case Square:
            osc.initialise ([] (float x)
            {
                return (x < 0.0f ? -1.0f : 1.0f);
            });
            break;

        case Triangle:
            osc.initialise ([] (float x)
            {
                return (2.0f / juce::MathConstants<float>::pi)
                        * std::asin (std::sin (x));
            });
            break;
    }
}
