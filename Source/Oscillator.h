//
// Created by alisdair chauvin on 12/2/25.
//

#ifndef EFFEM_UNIT_OSCILLATOR_H
#define EFFEM_UNIT_OSCILLATOR_H

#pragma once
#include <juce_dsp/juce_dsp.h>

class Oscillator {
public:
    Oscillator();

    //call from prepareToPlay
    void prepare(double sampleRate, int samplesPerBlock, int channels);

    //call from processBlock
    void process (juce::AudioBuffer<float>& buffer);

    void setFrequency(float newFreq);
    void setGain (float newGain);
    void setWaveform(int waveformIndex);

private:
    juce::dsp::Oscillator<float> osc;
    juce::dsp::Gain<float> gain;

    juce::dsp::ProcessSpec spec;

    enum Waveforms
    {
        Sine = 0,
        Saw,
        Square,
        Triangle
    };

    void initWaveform (int waveformIndex);
};


#endif //EFFEM_UNIT_OSCILLATOR_H