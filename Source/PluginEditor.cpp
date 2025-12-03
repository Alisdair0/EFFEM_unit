#include "PluginProcessor.h"
#include "PluginEditor.h"

static void configureSliderTwoDecimals(juce::Slider& s)
{
    s.setNumDecimalPlacesToDisplay(2);
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (900, 400);

    auto& state = processorRef.getState();

    // Play button
    addAndMakeVisible (playButton);
    playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        state, "play", playButton);

    // Master Gain (rotary)
    masterGainSlider.setSliderStyle (juce::Slider::Rotary);
    masterGainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (masterGainSlider);
    addAndMakeVisible (masterGainLabel);
    masterGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "masterGain", masterGainSlider);

    // Detune (vertical)
    detuneSlider.setSliderStyle (juce::Slider::LinearVertical);
    detuneSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (detuneSlider);
    addAndMakeVisible (detuneLabel);
    detuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "detune", detuneSlider);

    // Pitch Shift (combo)
    // Pitch Shift
    pitchShiftBox.addItem("-12 semitones", 1);
    pitchShiftBox.addItem("0 semitones", 2);
    pitchShiftBox.addItem("+12 semitones", 3);
    addAndMakeVisible(pitchShiftBox);

    pitchShiftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.getState(), "pitchShift", pitchShiftBox);

    pitchShiftLabel.setText("Pitch Shift", juce::dontSendNotification);
    pitchShiftLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchShiftLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(pitchShiftLabel);


    // Pan (rotary)
    panSlider.setSliderStyle (juce::Slider::Rotary);
    panSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (panSlider);
    addAndMakeVisible (panLabel);
    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "pan", panSlider);

    // FM Amount (rotary)
    fmSlider.setSliderStyle (juce::Slider::Rotary);
    fmSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (fmSlider);
    addAndMakeVisible (fmLabel);
    fmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "fmAmount", fmSlider);

    configureSliderTwoDecimals(detuneSlider);
    configureSliderTwoDecimals(panSlider);
    configureSliderTwoDecimals(masterGainSlider);
    configureSliderTwoDecimals(fmSlider);

    // ===================== ADSR Sliders ========================= //

    // Attack
    attackSlider.setSliderStyle(juce::Slider::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(attackSlider);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    state, "attack", attackSlider);

    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(attackLabel);

    // Decay
    decaySlider.setSliderStyle(juce::Slider::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(decaySlider);

    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "decay", decaySlider);

    decayLabel.setJustificationType(juce::Justification::centred);
    decayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(decayLabel);

    // Sustain
    sustainSlider.setSliderStyle(juce::Slider::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(sustainSlider);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "sustain", sustainSlider);

    sustainLabel.setJustificationType(juce::Justification::centred);
    sustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(sustainLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(releaseSlider);

    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "release", releaseSlider);

    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(releaseLabel);

    configureSliderTwoDecimals(attackSlider);
    configureSliderTwoDecimals(decaySlider);
    configureSliderTwoDecimals(sustainSlider);
    configureSliderTwoDecimals(releaseSlider);

    // label styling
    for (auto* label : { &masterGainLabel, &detuneLabel, &pitchShiftLabel, &panLabel, &fmLabel })
    {
        label->setColour (juce::Label::textColourId, juce::Colours::white);
        label->setJustificationType (juce::Justification::centred);
    }
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() = default;

//==============================================================================


void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);

    // top row: master gain, pan, FM
    auto topRow = area.removeFromTop (160);
    const int knobWidth = 140;

    auto mgArea = topRow.removeFromLeft (knobWidth);
    masterGainLabel.setBounds (mgArea.removeFromTop (20));
    masterGainSlider.setBounds (mgArea);

    auto panArea = topRow.removeFromLeft (knobWidth);
    panLabel.setBounds (panArea.removeFromTop (20));
    panSlider.setBounds (panArea);

    auto fmArea = topRow.removeFromLeft (knobWidth);
    fmLabel.setBounds (fmArea.removeFromTop (20));
    fmSlider.setBounds (fmArea);

    // middle: detune and pitch shift
    auto midRow = area.removeFromTop (160);

    auto detuneArea = midRow.removeFromLeft (knobWidth);
    detuneLabel.setBounds (detuneArea.removeFromTop (20));
    detuneSlider.setBounds (detuneArea);

    auto pitchArea = midRow.removeFromLeft (knobWidth);
    pitchShiftLabel.setBounds(20, 115, 120, 20);
    pitchShiftBox.setBounds(20, 135, 120, 24);

    // bottom: play button
    playButton.setBounds (area.removeFromTop (40).removeFromLeft (120));

    // ADSR layout (4 vertical sliders in a row)
    int left = 20;
    int top = 200;
    int width = 40;
    int height = 160;

    attackLabel .setBounds(left, top - 20, width, 20);
    attackSlider.setBounds(left, top, width, height);

    left += 50;
    decayLabel .setBounds(left, top - 20, width, 20);
    decaySlider.setBounds(left, top, width, height);

    left += 50;
    sustainLabel .setBounds(left, top - 20, width, 20);
    sustainSlider.setBounds(left, top, width, height);

    left += 50;
    releaseLabel .setBounds(left, top - 20, width, 20);
    releaseSlider.setBounds(left, top, width, height);
}
