#include "PluginProcessor.h"
#include "PluginEditor.h"

static void configureSliderTwoDecimals(juce::Slider& s)
{
    s.setNumDecimalPlacesToDisplay(2);
}

WaveformDisplay::WaveformDisplay(AudioPluginAudioProcessor& p)
    : processor(p)
{
    startTimerHz(60); // redraw at 60fps
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), waveformDisplay(p)
{
    setSize (850, 780);

    auto& state = processorRef.getState();

    addAndMakeVisible(waveformDisplay);

    // =========================================================
    // PLAY BUTTON
    // =========================================================
    addAndMakeVisible (playButton);
    playAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ButtonAttachment>(
            state, "play", playButton);

    // =========================================================
    // GLOBAL CONTROLS
    // =========================================================

    // Master Gain (global)
    masterGainSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    masterGainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (masterGainSlider);
    addAndMakeVisible (masterGainLabel);
    masterGainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "masterGain", masterGainSlider);

    // Pan (horizontal, lives in filter row)
    panSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    panSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (panSlider);
    addAndMakeVisible (panLabel);
    panAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "pan", panSlider);

    // Global FM amount (if you keep it)
    fmSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    fmSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (fmSlider);
    addAndMakeVisible (fmLabel);
    fmAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "fmAmount", fmSlider);

    configureSliderTwoDecimals(detuneSlider);
    configureSliderTwoDecimals(panSlider);
    configureSliderTwoDecimals(masterGainSlider);
    configureSliderTwoDecimals(fmSlider);

    // ADSR

    attackSlider.setSliderStyle(juce::Slider::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(attackLabel);

    attackAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "attack", attackSlider);

    decaySlider.setSliderStyle(juce::Slider::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(decayLabel);

    decayAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "decay", decaySlider);

    sustainSlider.setSliderStyle(juce::Slider::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(sustainLabel);

    sustainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "sustain", sustainSlider);

    releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 16);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(releaseLabel);

    releaseAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "release", releaseSlider);

    configureSliderTwoDecimals(attackSlider);
    configureSliderTwoDecimals(decaySlider);
    configureSliderTwoDecimals(sustainSlider);
    configureSliderTwoDecimals(releaseSlider);

    // =========================================================
    // FILTER SECTION (with Pan)
    // =========================================================

    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterLabel);

    filterType.addItem("Lowpass", 1);
    filterType.addItem("Highpass", 2);
    filterType.addItem("Bandpass", 3);
    addAndMakeVisible(filterType);

    filterTypeAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "filterType", filterType);

    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(cutoffSlider);
    addAndMakeVisible(cutoffLabel);

    cutoffAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "filterCutoff", cutoffSlider);

    resonanceSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(resonanceSlider);
    addAndMakeVisible(resonanceLabel);

    resonanceAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "filterResonance", resonanceSlider);

    // =========================================================
    // OSCILLATOR 1
    // =========================================================

    osc1WaveBox.addItemList({ "Sine","Square","Saw","Triangle","Noise","Add1","Add2" }, 1);
    addAndMakeVisible(osc1WaveBox);

    osc1PitchBox.addItem("-12", 1);
    osc1PitchBox.addItem("-7", 2);
    osc1PitchBox.addItem("0",  3);
    osc1PitchBox.addItem("+7", 4);
    osc1PitchBox.addItem("+12",5);
    addAndMakeVisible(osc1PitchBox);

    addAndMakeVisible(osc1GainLabel);
    addAndMakeVisible(osc1DetuneLabel);
    addAndMakeVisible(osc1FmLabel);
    addAndMakeVisible(osc1PitchLabel);
    addAndMakeVisible(osc1WaveLabel);

    detune1Slider.setSliderStyle(juce::Slider::LinearVertical);
    detune1Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(detune1Slider);

    gain1Slider.setSliderStyle(juce::Slider::LinearVertical);
    gain1Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(gain1Slider);

    fm1Slider.setSliderStyle(juce::Slider::LinearVertical);
    fm1Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(fm1Slider);

    osc1WaveAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "osc1Wave", osc1WaveBox);

    osc1PitchAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "osc1Pitch", osc1PitchBox);

    osc1DetuneAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "osc1Detune", detune1Slider);

    osc1GainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "osc1Gain", gain1Slider);

    osc1FmAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "osc1FM", fm1Slider);

    // =========================================================
    // OSCILLATOR 2
    // =========================================================

    osc2WaveBox.addItemList({ "Sine","Square","Saw","Triangle","Noise","Add1","Add2" }, 1);
    addAndMakeVisible(osc2WaveBox);

    osc2PitchBox.addItem("-12", 1);
    osc2PitchBox.addItem("-7", 2);
    osc2PitchBox.addItem("0",  3);
    osc2PitchBox.addItem("+7", 4);
    osc2PitchBox.addItem("+12",5);
    addAndMakeVisible(osc2PitchBox);

    addAndMakeVisible(osc2GainLabel);
    addAndMakeVisible(osc2DetuneLabel);
    addAndMakeVisible(osc2FmLabel);
    addAndMakeVisible(osc2PitchLabel);
    addAndMakeVisible(osc2WaveLabel);

    detune2Slider.setSliderStyle(juce::Slider::LinearVertical);
    detune2Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(detune2Slider);

    gain2Slider.setSliderStyle(juce::Slider::LinearVertical);
    gain2Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(gain2Slider);

    fm2Slider.setSliderStyle(juce::Slider::LinearVertical);
    fm2Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(fm2Slider);

    osc2WaveAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "osc2Wave", osc2WaveBox);

    osc2PitchAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            state, "osc2Pitch", osc2PitchBox);

    osc2DetuneAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "osc2Detune", detune2Slider);

    osc2GainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "osc2Gain", gain2Slider);

    osc2FmAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "osc2FM", fm2Slider);

    // =========================================================
    // BLEND SLIDER (between the two oscillators)
    // =========================================================

    blendLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(blendLabel);

    blendSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    blendSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(blendSlider);

    blendAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "oscBlend", blendSlider);

    // =============== LABEL STYLING ================= //
    for (auto* label : {
        &masterGainLabel, &detuneLabel, &pitchShiftLabel,
        &panLabel, &fmLabel, &attackLabel, &decayLabel,
        &sustainLabel, &releaseLabel, &filterLabel,
        &cutoffLabel, &resonanceLabel, &blendLabel,
        &osc1GainLabel, &osc1DetuneLabel, &osc1FmLabel,
        &osc1PitchLabel, &osc1WaveLabel,
        &osc2GainLabel, &osc2DetuneLabel, &osc2FmLabel,
        &osc2PitchLabel, &osc2WaveLabel
    })
    {
        label->setColour (juce::Label::textColourId, juce::Colours::white);
        label->setJustificationType (juce::Justification::centred);
    }
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() = default;

//==============================================================================


void WaveformDisplay::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);

    auto& data = processor.scopeData;
    int writePos = processor.scopeWritePos.load();

    juce::Path p;
    p.startNewSubPath(0, getHeight() / 2);

    for (int i = 0; i < AudioPluginAudioProcessor::scopeSize; ++i)
    {
        int index = (writePos + i) % AudioPluginAudioProcessor::scopeSize;

        float x = juce::jmap((float)i, 0.f,
                             (float)AudioPluginAudioProcessor::scopeSize - 1,
                             0.f, (float)getWidth());

        float y = juce::jmap(data[index],
                             -1.f, 1.f,
                             (float)getHeight(), 0.f);

        p.lineTo(x, y);
    }
    g.setColour(juce::Colours::orange);
    g.strokePath(p, juce::PathStrokeType(2.0f));
}

void WaveformDisplay::timerCallback()
{
    repaint();
}

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void AudioPluginAudioProcessorEditor::resized() {
    auto area = getLocalBounds().reduced(20);

    // ================= MASTER CONTROLS =================
    juce::Rectangle<int> masterRow = area.removeFromTop(60).reduced(20, 5);

    // Split into left and right sections
    auto leftHalf  = masterRow.removeFromLeft(masterRow.getWidth() / 2).reduced(10, 0);
    auto rightHalf = masterRow.reduced(10, 0);

    // Left: Master Gain
    masterGainLabel.setBounds(leftHalf.removeFromTop(20));
    masterGainSlider.setBounds(leftHalf);

    // Right: FM Amount
    fmLabel.setBounds(rightHalf.removeFromTop(20));
    fmSlider.setBounds(rightHalf);

    // Reserve top 150px for waveform
    auto waveformArea = area.removeFromTop(150).reduced(10);
    waveformDisplay.setBounds(waveformArea);

    // =========================================================
    // TOP: OSCILLATOR SECTION (horizontal per oscillator)
    // =========================================================
    auto oscArea = area.removeFromTop(150);

    auto osc1Area = oscArea.removeFromLeft(oscArea.getWidth() / 2).reduced(10);
    auto osc2Area = oscArea.reduced(10);

    // ------------- OSC 1 ------------- //
    {
        auto topRow = osc1Area.removeFromTop(30);
        osc1WaveBox.setBounds(topRow.removeFromLeft(120));
        osc1PitchBox.setBounds(topRow.removeFromLeft(70));

        auto labelY = osc1WaveBox.getY() - 16;
        osc1WaveLabel.setBounds(osc1WaveBox.getX(), labelY, 120, 16);
        osc1PitchLabel.setBounds(osc1PitchBox.getX(), labelY, 70, 16);

        auto knobRow = osc1Area.removeFromTop(90);

        detune1Slider.setBounds(knobRow.removeFromLeft(80).reduced(5));
        osc1DetuneLabel.setBounds(detune1Slider.getX(), detune1Slider.getY() - 16, 80, 16);

        gain1Slider.setBounds(knobRow.removeFromLeft(80).reduced(5));
        osc1GainLabel.setBounds(gain1Slider.getX(), gain1Slider.getY() - 16, 80, 16);

        fm1Slider.setBounds(knobRow.removeFromLeft(80).reduced(5));
        osc1FmLabel.setBounds(fm1Slider.getX(), fm1Slider.getY() - 16, 80, 16);
    }

    // ------------- OSC 2 ------------- //
    {
        auto topRow = osc2Area.removeFromTop(30);
        osc2WaveBox.setBounds(topRow.removeFromLeft(120));
        osc2PitchBox.setBounds(topRow.removeFromLeft(70));

        auto labelY = osc2WaveBox.getY() - 16;
        osc2WaveLabel.setBounds(osc2WaveBox.getX(), labelY, 120, 16);
        osc2PitchLabel.setBounds(osc2PitchBox.getX(), labelY, 70, 16);

        auto knobRow = osc2Area.removeFromTop(90);

        detune2Slider.setBounds(knobRow.removeFromLeft(80).reduced(5));
        osc2DetuneLabel.setBounds(detune2Slider.getX(), detune2Slider.getY() - 16, 80, 16);

        gain2Slider.setBounds(knobRow.removeFromLeft(80).reduced(5));
        osc2GainLabel.setBounds(gain2Slider.getX(), gain2Slider.getY() - 16, 80, 16);

        fm2Slider.setBounds(knobRow.removeFromLeft(80).reduced(5));
        osc2FmLabel.setBounds(fm2Slider.getX(), fm2Slider.getY() - 16, 80, 16);
    }

    // =========================================================
    // OSC BLEND (center)
    // =========================================================
    auto blendArea = area.removeFromTop(60);
    blendSlider.setBounds(blendArea.withSizeKeepingCentre(300, 40));
    blendLabel.setBounds(blendSlider.getX(), blendSlider.getY() - 16,
                         blendSlider.getWidth(), 16);

    // =========================================================
    // ADSR (Attack / Decay / Sustain / Release)
    // =========================================================
    auto adsrArea = area.removeFromTop(170).reduced(40);
    int adsrWidth = adsrArea.getWidth() / 4;

    attackSlider.setBounds(adsrArea.removeFromLeft(adsrWidth).reduced(10));
    attackLabel.setBounds(attackSlider.getX(), attackSlider.getY() - 16,
                          attackSlider.getWidth(), 16);

    decaySlider.setBounds(adsrArea.removeFromLeft(adsrWidth).reduced(10));
    decayLabel.setBounds(decaySlider.getX(), decaySlider.getY() - 16,
                         decaySlider.getWidth(), 16);

    sustainSlider.setBounds(adsrArea.removeFromLeft(adsrWidth).reduced(10));
    sustainLabel.setBounds(sustainSlider.getX(), sustainSlider.getY() - 16,
                           sustainSlider.getWidth(), 16);

    releaseSlider.setBounds(adsrArea.removeFromLeft(adsrWidth).reduced(10));
    releaseLabel.setBounds(releaseSlider.getX(), releaseSlider.getY() - 16,
                           releaseSlider.getWidth(), 16);

    // =========================================================
    // FILTER SECTION + PAN SLIDER
    // =========================================================
    auto filterArea = area.removeFromTop(180);

    filterType.setBounds(filterArea.removeFromTop(30).withSizeKeepingCentre(140, 30));
    filterLabel.setBounds(filterType.getX(), filterType.getY() - 16,
                          filterType.getWidth(), 16);

    cutoffSlider.setBounds(filterArea.removeFromTop(50).reduced(20));
    cutoffLabel.setBounds(cutoffSlider.getX(), cutoffSlider.getY() - 16,
                          cutoffSlider.getWidth(), 16);

    resonanceSlider.setBounds(filterArea.removeFromTop(50).reduced(20));
    resonanceLabel.setBounds(resonanceSlider.getX(), resonanceSlider.getY() - 16,
                             resonanceSlider.getWidth(), 16);

    panSlider.setBounds(filterArea.removeFromTop(40).withSizeKeepingCentre(220, 30));
    panLabel.setBounds(panSlider.getX(), panSlider.getY() - 16,
                       panSlider.getWidth(), 16);

    // =========================================================
    // BOTTOM: MASTER CONTROLS (Gain / FM / Detune)
    // =========================================================
    {
        auto bottomArea = area.removeFromBottom(150).reduced(40);

        int columnWidth = bottomArea.getWidth() / 3;

    }
}




