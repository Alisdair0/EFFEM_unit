#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p),
      freqSliderAttachment(processorRef.getState(), "freqHz", frequencySlider),
      playButtomAttachment(processorRef.getState(), "play", playButton)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    //addAndMakeVisible (square);

    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    //frequencySlider.setRange(0.0f, 1.0f, 0.01f);
    addAndMakeVisible(frequencySlider);

    // === Play Button === //

    playButton.setButtonText("Playing");
    playButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    playButton.setClickingTogglesState(true);

    playButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    playButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::red);

    playButton.onClick = [this]()
    {
        // Change the state of button when it's clicked
        const bool isPlaying = playButton.getToggleState();
        playButton.setButtonText(isPlaying ? "Playing" : "Bypassed");
    };
    // ==================== //

    addAndMakeVisible(playButton);

    //frequencyLabel.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::white); //outline color
    frequencyLabel.setJustificationType(juce::Justification::centred);;
    addAndMakeVisible(frequencyLabel);

    setSize (400, 400);

    // === Waveform Selector === //
    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Saw", 2);
    waveformSelector.addItem("Triangle", 3);
    waveformSelector.setSelectedId(1); // default: Sine

    waveformSelector.onChange = [this]() {
        switch (waveformSelector.getSelectedId())
        {
            case 1: processorRef.setWaveform(AudioPluginAudioProcessor::Waveform::Sine); break;
            case 2: processorRef.setWaveform(AudioPluginAudioProcessor::Waveform::Saw); break;
            case 3: processorRef.setWaveform(AudioPluginAudioProcessor::Waveform::Triangle); break;
        }
    };

    addAndMakeVisible(waveformSelector);

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    //g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText (""
    //                  "EFFEM", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor.
    //square.setBounds (100, 100, 200, 200);
    frequencyLabel.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 120, 100, 20);
    frequencySlider.setBounds (getWidth() / 2 - 50, getHeight() / 2 - 100, 100, 200);
    playButton.setBounds (getWidth() / 2 - 50, getHeight() / 2 + 120, 100, 20);
    waveformSelector.setBounds(getWidth()/2 - 50, getHeight()/2 + 160, 100, 20);
}
