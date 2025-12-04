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
      ), state (*this, nullptr, "parameters", createParameters())
{
    synth.clearVoices();
    for (int i = 0; i < 8; ++i)         // 8-voice polyphony
        synth.addVoice (new SynthVoice);

    synth.clearSounds();
    synth.addSound (new SynthSound);
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
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const int numCh = getTotalNumOutputChannels();
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            v->prepare(sampleRate, samplesPerBlock, numCh);   // USE numCh
    }

    // ======== Parameters ============ //

    // Buttons & sliders
    playParam = state.getRawParameterValue ("play");
    masterGainParam = state.getRawParameterValue ("masterGain");
    detuneParam = state.getRawParameterValue ("detune");
    pitchChoiceParam = state.getRawParameterValue ("pitchShift");
    panParam = state.getRawParameterValue ("pan");
    fmAmountParam = state.getRawParameterValue ("fmAmount");

    // Envelopes
    attackParam  = state.getRawParameterValue ("attack");
    decayParam   = state.getRawParameterValue ("decay");
    sustainParam = state.getRawParameterValue ("sustain");
    releaseParam = state.getRawParameterValue ("release");

    // Filter
    filterCutoffParam    = state.getRawParameterValue("filterCutoff");
    filterResonanceParam = state.getRawParameterValue("filterResonance");
    filterTypeParam      = state.getRawParameterValue("filterType");
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    buffer.clear();

    // read global parameters
    const float masterGain = masterGainParam ? masterGainParam->load() : 0.8f;
    const float detuneCents= detuneParam ? detuneParam->load() : 0.0f;
    const float pan = panParam ? panParam->load() : 0.0f;
    const float fmAmount = fmAmountParam ? fmAmountParam->load() : 0.0f;

    const float attack  = attackParam  ? attackParam->load()  : 0.01f;
    const float decay   = decayParam   ? decayParam->load()   : 0.1f;
    const float sustain = sustainParam ? sustainParam->load() : 0.8f;
    const float release = releaseParam ? releaseParam->load() : 0.2f;

    const float cutoff    = filterCutoffParam    ? filterCutoffParam->load()    : 1000.0f;
    const float resonance = filterResonanceParam ? filterResonanceParam->load() : 0.2f;
    const int   type      = filterTypeParam      ? (int)filterTypeParam->load() : 0;

    auto* wfParam = state.getRawParameterValue ("waveform");
    int waveformIndex = 0;

    if (wfParam != nullptr)
        waveformIndex = (int) std::round (*wfParam);

    int pitchIndex = (int) pitchChoiceParam->load();
    int mappedIndex = pitchIndex - 1;  // -> 0, 1, 2
    float pitchShiftSemis = mappedIndex * 12.0f;

    if (pitchChoiceParam != nullptr)
        pitchIndex = (int) std::round (pitchChoiceParam->load());

    auto* w1p = state.getRawParameterValue("osc1Waveform");
    auto* w2p = state.getRawParameterValue("osc2Waveform");
    auto* blp = state.getRawParameterValue("oscBlend");

    int wave1 = w1p ? (int)*w1p : 0;
    int wave2 = w2p ? (int)*w2p : 0;
    float blendAmount = blp ? (float)*blp : 0.5f;

    // update all voices with these params
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
        {
            v->updateFromParameters (masterGain, detuneCents, pitchShiftSemis, fmAmount);
            v->updateEnvelope       (attack, decay, sustain, release);
            v->updateFilter         (cutoff, resonance, type);
            v->updateOscillators(wave1, wave2, blendAmount);
        }
    }

    // render synth
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // apply pan (simple stereo pan)
    if (buffer.getNumChannels() >= 2)
    {
        const float angle = (pan + 1.0f) * juce::MathConstants<float>::pi / 4.0f;
        const float leftGain  = std::cos (angle);
        const float rightGain = std::sin (angle);

        auto* left  = buffer.getWritePointer (0);
        auto* right = buffer.getWritePointer (1);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            left[i]  *= leftGain;
            right[i] *= rightGain;
        }
    }

    // global mute if play is off
    if (playParam && ! static_cast<bool> (playParam->load()))
        buffer.clear();
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

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
    "pitchShift",
    "Pitch Shift",
    juce::StringArray{ "+12", "0", "-12" },
    1));

    // ======= SLIDERS & BUTTONS ====== //
    params.push_back (std::make_unique<AudioParameterBool>(
        "play", "Play", true));

    params.push_back (std::make_unique<AudioParameterFloat>(
        "masterGain", "Master Gain",
        NormalisableRange<float> (0.0f, 1.0f, 0.001f, 0.5f),
        0.8f));

    params.push_back (std::make_unique<AudioParameterFloat>(
        "detune", "Detune (cents)",
        -100.0f, 100.0f, 0.0f));

    params.push_back (std::make_unique<AudioParameterFloat>(
        "pan", "Pan",
        -1.0f, 1.0f, 0.0f)); // -1 = left, 0 = center, +1 = right

    params.push_back (std::make_unique<AudioParameterFloat>(
        "fmAmount", "FM Amount",
        0.0f, 10.0f, 0.0f)); // simple frequency-offset style "FM"

    // ============== ADSR =================== //
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
    "attack", "Attack",
    juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.5f),
    0.01f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "decay", "Decay",
        juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.5f),
        0.1f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "sustain", "Sustain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
        0.8f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "release", "Release",
        juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.5f),
        0.2f));

    // ========== FILTER CONTROLS ========== //
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.33f), 1000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterResonance", "Resonance",
        juce::NormalisableRange<float>(0.1f, 1.5f, 0.001f), 0.2f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "filterType", "Filter Type",
        juce::StringArray { "Lowpass", "Highpass", "Bandpass" },
        0     // 0 = LP, 1 = HP, 2 = BP
    ));

    // ================= OSCILLATOR 1 WAVEFORM ================= //
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        "osc1Waveform", "Osc1 Waveform",
        juce::StringArray { "Sine", "Square", "Saw", "Triangle", "Noise", "Add1", "Add2" },
        0 // default
    ));

    // ================= OSCILLATOR 2 WAVEFORM ================= //
    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        "osc2Waveform", "Osc2 Waveform",
        juce::StringArray { "Sine", "Square", "Saw", "Triangle", "Noise", "Add1", "Add2" },
        0 // default
    ));

    // ================= OSC BLEND ================= //
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "oscBlend", "Osc Blend",
        juce::NormalisableRange<float>(0.0f, 1.0f),
        0.5f    // 0.0 = pure osc1, 1.0 = pure osc2
    ));

    return { params.begin(), params.end() };
}