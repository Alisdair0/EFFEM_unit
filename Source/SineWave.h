//
// Created by alisdair chauvin on 11/18/25.
//

#ifndef SIMPLEJUCEPLUGINTEMPLATE_SINEWAVE_H
#define SIMPLEJUCEPLUGINTEMPLATE_SINEWAVE_H

#include <juce_audio_basics/juce_audio_basics.h>

class SineWave {
public:
    void prepare (double sampleRate, int numChannels);
    void process (juce::AudioBuffer<float>& buffer);

    [[nodiscard]] float getAmplitude() const { return amplitude; }
    [[nodiscard]] float getFrequency() const { return frequency; }
    void setAmplitude(const float newAmplitude) { amplitude = newAmplitude; }
    void setFrequency(const float newFrequency) { frequency = newFrequency; }


private:
    float amplitude = 0.00f;
    float frequency = 440.0f;
    float currentSampleRate = 0.0f;
    float timeIncrement = 0.0f;
    std::vector<float> currentTime;
    //float iteration = 0.0f;
};


#endif //SIMPLEJUCEPLUGINTEMPLATE_SINEWAVE_H