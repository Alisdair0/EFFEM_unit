//
// Created by alisdair chauvin on 11/18/25.
//

#include "SineWave.h"

void SineWave::prepare (const double sampleRate, const int numChannels) {
    currentSampleRate = static_cast<float>(sampleRate);
    timeIncrement = 1.0f / currentSampleRate;
    currentTime.resize(numChannels, 0.0f);
}

void SineWave::process (juce::AudioBuffer<float>& buffer) {
    if (currentTime.size() != buffer.getNumChannels()) {
        return;
    }

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {

        // Access info inside AudioBuffer and output sine wave
        auto* output = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            output[sample] = amplitude * std::sinf(juce::MathConstants<float>::twoPi * frequency * currentTime[channel]);
            currentTime[channel] += timeIncrement;
            // iteration++;
            // std::cout << "Channel: " << channel << " Current Time: " << iteration << " / " << currentSampleRate << std::endl;
        }
    }
}