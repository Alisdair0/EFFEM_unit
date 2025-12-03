#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

//==============================================================================

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    // ======================
    // OSCILLATOR CONTROLS
    // ======================
    juce::Slider baseFreqSlider;
    juce::Label baseFreqLabel { "BaseFreqLabel", "Base Frequency" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> baseFreqAttachment;

    juce::Slider osc1GainSlider;
    juce::Label osc1GainLabel { "Osc1GainLabel", "Osc 1 Gain" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1GainAttachment;

    juce::Slider osc2GainSlider;
    juce::Label osc2GainLabel { "Osc2GainLabel", "Osc 2 Gain" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2GainAttachment;

    juce::Slider detuneSlider;
    juce::Label detuneLabel { "DetuneLabel", "Detune (cents)" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;

    juce::Slider pitchShiftSlider;
    juce::Label pitchShiftLabel { "PitchShiftLabel", "Pitch Shift (st)" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchShiftAttachment;

    juce::Slider harmonicsSlider;
    juce::Label harmonicsLabel { "HarmonicsLabel", "Harmonics" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> harmonicsAttachment;

    juce::ComboBox osc2ModeBox;
    juce::Label osc2ModeLabel { "Osc2ModeLabel", "Osc 2 Mode" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2ModeAttachment;

    juce::ToggleButton playButton { "Play" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> playAttachment;

    // ======================
    // FUTURE UI SECTIONS
    // (MIDI, ADSR, FILTERS, VISUALIZER)
    // ======================
    juce::GroupComponent envelopesGroup { "envGroup", "Envelopes (Coming Soon)" };
    juce::GroupComponent filtersGroup   { "filterGroup", "Filters (Coming Soon)" };
    juce::GroupComponent midiGroup      { "midiGroup", "MIDI Settings (Coming Soon)" };
    juce::GroupComponent visualGroup    { "visualGroup", "Visualizer (Coming Soon)" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};