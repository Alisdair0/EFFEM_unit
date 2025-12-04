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
    setSize (1250, 450);

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

    // --- FILTER LABEL ---
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterLabel);

    // --- FILTER TYPE ---
    filterType.addItem("Lowpass", 1);
    filterType.addItem("Highpass", 2);
    filterType.addItem("Bandpass", 3);
    addAndMakeVisible(filterType);

    // APS parameters = your APVTS
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        state, "filterType", filterType);

    // --- CUTOFF SLIDER ---
    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(cutoffSlider);

    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "filterCutoff", cutoffSlider);

    // --- RESONANCE SLIDER ---
    resonanceSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(resonanceSlider);

    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        state, "filterResonance", resonanceSlider);

    // ---- OSC 1 ----
    osc1WaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc1WaveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(osc1WaveLabel);

    osc1WaveBox.addItem("Sine", 1);
    osc1WaveBox.addItem("Square", 2);
    osc1WaveBox.addItem("Saw", 3);
    osc1WaveBox.addItem("Triangle", 4);
    osc1WaveBox.addItem("Noise", 5);
    osc1WaveBox.addItem("Add1", 6);
    osc1WaveBox.addItem("Add2", 7);
    addAndMakeVisible(osc1WaveBox);

    osc1WaveAttach = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "osc1Waveform", osc1WaveBox);

    // ---- OSC 2 ----
    osc2WaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2WaveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(osc2WaveLabel);

    osc2WaveBox.addItem("Sine", 1);
    osc2WaveBox.addItem("Square", 2);
    osc2WaveBox.addItem("Saw", 3);
    osc2WaveBox.addItem("Triangle", 4);
    osc2WaveBox.addItem("Noise", 5);
    osc2WaveBox.addItem("Add1", 6);
    osc2WaveBox.addItem("Add2", 7);
    addAndMakeVisible(osc2WaveBox);

    osc2WaveAttach = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "osc2Waveform", osc2WaveBox);

    // ---- BLEND ----
    blendLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    blendLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(blendLabel);

    blendSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    blendSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(blendSlider);

    blendAttach = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "oscBlend", blendSlider);

    // =============== LABEL STYLING ================= //
    for (auto* label : { &masterGainLabel, &detuneLabel, &pitchShiftLabel,
                         &panLabel, &fmLabel, &attackLabel, &decayLabel,
                         &sustainLabel, &releaseLabel, &filterLabel,
                         &cutoffLabel, &resonanceLabel })
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
    const int pad      = 20;
    const int labelH   = 20;
    const int controlH = 55;
    const int colW     = 120;

    auto place = [&](juce::Label& l, juce::Component& c, int x, int y)
    {
        l.setBounds(x, y, colW, labelH);
        c.setBounds(x, y + labelH, colW, controlH);
    };

    // === ROW 1: Gain | Waveform | Detune | PitchShift | Pan | FM | Release ===
    int y1 = pad;
    int x  = pad;

    place(osc1WaveLabel, osc1WaveBox,  x, y1);  x += colW + pad;
    place(osc2WaveLabel, osc2WaveBox,  x, y1);  x += colW + pad;
    place(blendLabel,    blendSlider,  x, y1);  x += colW + pad;
    place(masterGainLabel, masterGainSlider, x, y1);  x += colW + pad;
    place(detuneLabel, detuneSlider,         x, y1);  x += colW + pad;
    place(pitchShiftLabel, pitchShiftBox,    x, y1);  x += colW + pad;
    place(panLabel, panSlider,               x, y1);  x += colW + pad;
    place(fmLabel, fmSlider,                 x, y1);

    // === ROW 2: Attack | Decay | Sustain ===
    int y2 = y1 + labelH + controlH + pad;
    x = pad;

    place(attackLabel, attackSlider,   x, y2);  x += colW + pad;
    place(decayLabel,  decaySlider,    x, y2);  x += colW + pad;
    place(sustainLabel, sustainSlider, x, y2);  x += colW + pad;
    place(releaseLabel, releaseSlider, x, y1);

    // === ROW 3: Filter | Cutoff | Resonance ===
    int y3 = y2 + labelH + controlH + pad;
    x = pad;

    place(filterLabel, filterType,     x, y3);  x += colW + pad;
    place(cutoffLabel, cutoffSlider,   x, y3);  x += colW + pad;
    place(resonanceLabel, resonanceSlider, x, y3);
}



