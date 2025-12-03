#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), state(*this, nullptr, "parameters", createParameters())
{
}


AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    osc1.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    osc2.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    osc1.setWaveform(0);
    osc2.setWaveform(0);

    // Parameter pointers
    osc1GainParam = state.getRawParameterValue("osc1Gain");
    osc2GainParam = state.getRawParameterValue("osc2Gain");

    detuneParam   = state.getRawParameterValue("detune");
    pitchParam    = state.getRawParameterValue("pitchShift");
    harmonicsParam= state.getRawParameterValue("harmonics");
    osc2ModeParam = state.getRawParameterValue("osc2Mode");

    frequencyParam = state.getRawParameterValue("freqHz");
    playParam      = state.getRawParameterValue("play");
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    buffer.clear();

    if (! playParam->load())
        return;

    float baseFreq = frequencyParam->load();
    float detuned  = baseFreq + detuneParam->load();

    // pitch shift in semitones
    float pitchFactor = std::pow(2.0f, pitchParam->load() / 12.0f);

    // Apply harmonics later

    osc1.setFrequency(detuned * pitchFactor);
    osc2.setFrequency(detuned * pitchFactor);

    // Mode switch for osc2
    switch ((int)osc2ModeParam->load())
    {
        case 1: osc2.setFrequency(baseFreq / 2.0f); break;   // SUB
        case 2: osc2.setFrequency(baseFreq * 1.5f); break;   // FIFTH
        default: break;
    }

    osc1.setGain(osc1GainParam->load());
    osc2.setGain(osc2GainParam->load());

    // temp buffers
    juce::AudioBuffer<float> osc1Buf, osc2Buf;
    osc1Buf.setSize(buffer.getNumChannels(), buffer.getNumSamples());
    osc2Buf.setSize(buffer.getNumChannels(), buffer.getNumSamples());
    osc1Buf.clear();
    osc2Buf.clear();

    osc1.process(osc1Buf);
    osc2.process(osc2Buf);

    // Mix oscillators
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* out = buffer.getWritePointer(ch);
        auto* o1  = osc1Buf.getReadPointer(ch);
        auto* o2  = osc2Buf.getReadPointer(ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
            out[i] = o1[i] + o2[i];
    }

    // TO DO: apply harmonics processing
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout  AudioPluginAudioProcessor::createParameters() {
    return {
        std::make_unique<juce::AudioParameterFloat>("freqHz", "Frequency", 20.0f, 20000.0f, 220.0f),
        std::make_unique<juce::AudioParameterBool>("play", "Play", true),

        std::make_unique<juce::AudioParameterFloat>("osc1Gain", "Osc 1 Gain", 0.0f, 1.0f, 0.8f),
        std::make_unique<juce::AudioParameterFloat>("osc2Gain", "Osc 2 Gain", 0.0f, 1.0f, 0.4f),
        std::make_unique<juce::AudioParameterFloat>("detune", "Detune", -50.0f, 50.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("pitchShift", "Pitch Shift (Semitones)", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("harmonics", "Harmonics", 1.0f, 10.0f, 1.0f),
        std::make_unique<juce::AudioParameterChoice>("osc2Mode", "Osc 2 Mode", juce::StringArray{ "Normal", "Sub", "Fifth" }, 0)
    };
}