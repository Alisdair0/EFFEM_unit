#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef(p)
{
    setSize (900, 600);

    auto& state = processorRef.getState();

    // ======================
    // OSCILLATOR UI CREATION
    // ======================

    baseFreqSlider.setSliderStyle(juce::Slider::Rotary);
    baseFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(baseFreqSlider);
    addAndMakeVisible(baseFreqLabel);
    baseFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "freqHz", baseFreqSlider);

    osc1GainSlider.setSliderStyle(juce::Slider::Rotary);
    osc1GainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(osc1GainSlider);
    addAndMakeVisible(osc1GainLabel);
    osc1GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "osc1Gain", osc1GainSlider);

    osc2GainSlider.setSliderStyle(juce::Slider::Rotary);
    osc2GainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(osc2GainSlider);
    addAndMakeVisible(osc2GainLabel);
    osc2GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "osc2Gain", osc2GainSlider);

    detuneSlider.setSliderStyle(juce::Slider::Rotary);
    detuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(detuneSlider);
    addAndMakeVisible(detuneLabel);
    detuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "detune", detuneSlider);

    pitchShiftSlider.setSliderStyle(juce::Slider::Rotary);
    pitchShiftSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(pitchShiftSlider);
    addAndMakeVisible(pitchShiftLabel);
    pitchShiftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "pitchShift", pitchShiftSlider);

    harmonicsSlider.setSliderStyle(juce::Slider::Rotary);
    harmonicsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    addAndMakeVisible(harmonicsSlider);
    addAndMakeVisible(harmonicsLabel);
    harmonicsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "harmonics", harmonicsSlider);

    osc2ModeBox.addItem("Normal", 1);
    osc2ModeBox.addItem("Sub", 2);
    osc2ModeBox.addItem("Fifth", 3);
    addAndMakeVisible(osc2ModeBox);
    addAndMakeVisible(osc2ModeLabel);
    osc2ModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "osc2Mode", osc2ModeBox);

    addAndMakeVisible(playButton);
    playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "play", playButton);

    // ======================
    // FUTURE UI SECTIONS
    // ======================

    addAndMakeVisible(envelopesGroup);
    addAndMakeVisible(filtersGroup);
    addAndMakeVisible(midiGroup);
    addAndMakeVisible(visualGroup);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}
//==============================================================================
void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(15);

    // ======================
    // TOP: Oscillators
    // ======================
    auto oscArea = area.removeFromTop(200);

    auto row = oscArea.removeFromTop(180);
    auto dialWidth = 120;

    baseFreqSlider .setBounds(row.removeFromLeft(dialWidth));
    osc1GainSlider .setBounds(row.removeFromLeft(dialWidth));
    osc2GainSlider .setBounds(row.removeFromLeft(dialWidth));
    detuneSlider   .setBounds(row.removeFromLeft(dialWidth));
    pitchShiftSlider.setBounds(row.removeFromLeft(dialWidth));
    harmonicsSlider.setBounds(row.removeFromLeft(dialWidth));

    osc2ModeBox.setBounds(oscArea.removeFromLeft(150).reduced(10));
    playButton.setBounds(oscArea.removeFromLeft(100).reduced(10));

    // ======================
    // MIDDLE: Envelopes + Filters
    // ======================
    auto midArea = area.removeFromTop(180);
    envelopesGroup.setBounds(midArea.removeFromLeft(getWidth() / 2).reduced(5));
    filtersGroup.setBounds(midArea.reduced(5));

    // ======================
    // BOTTOM: MIDI + Visualizer
    // ======================
    auto bottomArea = area;
    midiGroup.setBounds(bottomArea.removeFromLeft(getWidth() / 2).reduced(5));
    visualGroup.setBounds(bottomArea.reduced(5));
}