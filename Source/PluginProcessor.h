#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Oscillator.h"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;


    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getState() { return state; }


private:
    Oscillator osc1, osc2;

    std::atomic<float>* frequencyParam = nullptr;
    std::atomic<float>* playParam = nullptr;

    std::atomic<float>* osc1GainParam = nullptr;
    std::atomic<float>* osc2GainParam = nullptr;
    std::atomic<float>* detuneParam = nullptr;
    std::atomic<float>* pitchParam = nullptr;
    std::atomic<float>* harmonicsParam = nullptr;
    std::atomic<float>* osc2ModeParam = nullptr;

    //SineWave sinewave;
    juce::AudioProcessorValueTreeState state;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};