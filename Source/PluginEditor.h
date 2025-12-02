#pragma once

#include "PluginProcessor.h"
#include "Square.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //Square square;

    juce::Slider frequencySlider;
    juce::TextButton playButton;
    juce::Label frequencyLabel{"FrequencyLabel", "Frequency"};

    juce::AudioProcessorValueTreeState::SliderAttachment freqSliderAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
