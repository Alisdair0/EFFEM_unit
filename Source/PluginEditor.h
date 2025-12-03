#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

//==============================================================================

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    // Play
    juce::ToggleButton playButton { "Play" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> playAttachment;

    // Master Gain
    juce::Slider masterGainSlider;
    juce::Label masterGainLabel { "MasterGainLabel", "Master Gain" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainAttachment;

    // Detune (vertical)
    juce::Slider detuneSlider;
    juce::Label detuneLabel { "DetuneLabel", "Detune (cents)" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;

    // Pitch Shift (combo)
    juce::ComboBox pitchShiftBox;
    juce::Label pitchShiftLabel { "PitchShiftLabel", "Pitch Shift" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> pitchShiftAttachment;

    // Pan
    juce::Slider panSlider;
    juce::Label panLabel { "PanLabel", "Pan" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment;

    // FM Amount
    juce::Slider fmSlider;
    juce::Label fmLabel { "FMLabel", "FM Amount" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fmAttachment;

    // ADSR sliders + labels
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;

    juce::Label attackLabel  { "attackLabel",  "A" };
    juce::Label decayLabel   { "decayLabel",   "D" };
    juce::Label sustainLabel { "sustainLabel", "S" };
    juce::Label releaseLabel { "releaseLabel", "R" };

    // attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    // Filters
    juce::Label filterLabel;
    juce::ComboBox filterType;

    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
