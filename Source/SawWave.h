//
// Created by alisdair chauvin on 11/18/25.
//

#ifndef SIMPLEJUCEPLUGINTEMPLATE_SAWWAVE_H
#define SIMPLEJUCEPLUGINTEMPLATE_SAWWAVE_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <numbers>

class SawWave {
public:
    void prepare (double sampleRate, int numChannels);
    void process (juce::AudioBuffer<float>& buffer);

    [[nodiscard]] float getAmplitude() const { return amplitude; }
    [[nodiscard]] float getFrequency() { return smoothedFreq.getNextValue(); }
    void setAmplitude(const float newAmplitude) { amplitude = newAmplitude; }
    void setFrequency(const float newFrequency) { smoothedFreq.setTargetValue(newFrequency); }


private:
    std::vector<float> phases;
    //float iteration = 0.0f;
    float amplitude = 0.5f;
    float sampleRate = 0.0f;
    //float timeIncrement = 0.0f;
    static constexpr float doublePi = 2.0f * std::numbers::pi_v<float>;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> smoothedFreq;


};

#endif //SIMPLEJUCEPLUGINTEMPLATE_SAWWAVE_H

