//
// Created by alisdair chauvin on 12/2/25.
//

#include "Oscillator.h"

Oscillator::Oscillator() {
    initWaveform(Sine);
    gain.setGainLinear(0.5f);
}

void Oscillator::prepare(double sampleRate, int samplesPerBlock, int numChannels) {
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = numChannels;

    osc.prepare(spec);
    gain.prepare(spec);
}

void Oscillator::process (juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> audioBlock (buffer);
    juce::dsp::ProcessContextReplacing<float> context (audioBlock);

    osc.process (context);
    gain.process (context);
}

void Oscillator::setFrequency (float newFreq)
{
    osc.setFrequency (newFreq);
}

void Oscillator::setGain (float newGain)
{
    gain.setGainLinear (newGain);
}

void Oscillator::setWaveform (int waveformIndex)
{
    initWaveform (waveformIndex);
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
