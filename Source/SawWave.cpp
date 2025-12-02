//
// Created by alisdair chauvin on 11/18/25.
//

#include "SawWave.h"

void SawWave::prepare (const double currentSampleRate, const int numChannels) {
    sampleRate = static_cast<float>(currentSampleRate);
    phases.resize(static_cast<size_t>(numChannels), 0.0f);
    smoothedFreq.reset(sampleRate, .01f);
    smoothedFreq.setCurrentAndTargetValue(getFrequency());
}

void SawWave::process (juce::AudioBuffer<float>& buffer) {
    assert(sampleRate > 0.0f);

    assert(amplitude >= 0.0f && amplitude <= 1.0f);
    assert(static_cast<size_t>(buffer.getNumChannels()) == phases.size());

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {

        // Access info inside AudioBuffer and output sine wave
        auto* channelData = buffer.getWritePointer(channel);
        auto& phase = phases[static_cast<size_t>(channel)];

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            const float frequency = smoothedFreq.getNextValue();
            const float phaseInc = (doublePi * frequency / sampleRate);

            channelData[sample] = amplitude * std::sin(phase);
            phase += phaseInc;
            // iteration++;
            // std::cout << "Channel: " << channel << " Current Time: " << iteration << " / " << currentSampleRate << std::endl;

            if (phase >= doublePi) {
                phase -= doublePi;
            }
        }
    }
}