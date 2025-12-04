//
// Created by alisdair chauvin on 12/2/25.
//

#ifndef EFFEM_UNIT_SYNTHVOICE_H
#define EFFEM_UNIT_SYNTHVOICE_H

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SynthSound.h"
#include "Oscillator.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() = default;

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*pitchWheelPosition*/) override
    {
        baseFrequency = static_cast<float> (
            juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));

        level   = velocity;
        isActive = true;
        adsr.noteOn();
    }

    void stopNote (float velocity, bool allowTailOff) override
    {
        juce::ignoreUnused (velocity);

        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            adsr.reset();
            isActive = false;
            clearCurrentNote();
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void prepare (double sampleRate, int samplesPerBlock, int numChannels)
    {
        osc1.prepare(sampleRate, samplesPerBlock, numChannels);
        osc2.prepare(sampleRate, samplesPerBlock, numChannels);

        osc1.setWaveform (0);   // sine (or whatever index 0 is)
        osc2.setWaveform (0);

        filterSpec.sampleRate = sampleRate;
        filterSpec.maximumBlockSize = samplesPerBlock;
        filterSpec.numChannels = (juce::uint32) numChannels;

        filter.prepare(filterSpec);

        // default LP
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

        adsr.setSampleRate(sampleRate);
    }

    // called once per block from processor
    void updateFromParameters (float masterGain,
                               float detuneCents,
                               float pitchShiftSemis,
                               float fmAmountParam)
    {
        gain       = masterGain;
        detune     = detuneCents;
        pitchShift = pitchShiftSemis;
        fmAmount   = fmAmountParam;


        const float pitchFactor = std::pow (2.0f, pitchShift / 12.0f);
        const float pitchedFreq = baseFrequency * pitchFactor;

        // osc1 at pitched frequency
        float osc1Freq = pitchedFreq;

        // osc2 slightly detuned and "FM-ish"
        const float detuneRatio = std::pow (2.0f, detune / 1200.0f);

        // interpret fmAmount as a frequency offset ratio
        const float fmRatio = 1.0f + (fmAmount * 0.1f); // 0..~2

        float osc2Freq = pitchedFreq * detuneRatio * fmRatio;

        osc1.setFrequency (osc1Freq);
        osc2.setFrequency (osc2Freq);

        // split gain between the two oscillators
        const float perOscGain = gain * 0.5f;
        osc1.setGain (perOscGain);
        osc2.setGain (perOscGain);
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample, int numSamples) override
    {
        if (! isActive)
            return;

        const int outputChans = outputBuffer.getNumChannels();

        tempBuffer1.setSize (outputChans, numSamples, false, false, true);
        tempBuffer2.setSize (outputChans, numSamples, false, false, true);
        mixBuffer  .setSize (outputChans, numSamples, false, false, true);


        tempBuffer1.clear();
        tempBuffer2.clear();
        mixBuffer.clear();

        // render both oscillators
        osc1.process (tempBuffer1);
        osc2.process (tempBuffer2);

        // mix into mixBuffer
        const int numChannels = outputBuffer.getNumChannels();
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* dst = mixBuffer.getWritePointer (ch);
            auto* s1  = tempBuffer1.getReadPointer (ch);
            auto* s2  = tempBuffer2.getReadPointer (ch);

            for (int i = 0; i < numSamples; ++i)
                dst[i] = (s1[i] * (1.0f - blend) + s2[i] * blend) * level;
        }

        // apply ADSR
        adsr.applyEnvelopeToBuffer (mixBuffer, 0, numSamples);

        // Apply filter to mixBuffer
        juce::dsp::AudioBlock<float> block(mixBuffer);
        filter.process(juce::dsp::ProcessContextReplacing<float>(block));

        // Add filtered result to output
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* out = outputBuffer.getWritePointer(ch, startSample);
            auto* src = mixBuffer.getReadPointer(ch);

            for (int i = 0; i < numSamples; ++i)
                out[i] += src[i];
        }

        // if envelope finished, kill the voice
        if (! adsr.isActive())
        {
            isActive = false;
            clearCurrentNote();
        }
    }


    void updateEnvelope (float attack, float decay, float sustain, float release)
    {
        adsrParams.attack  = attack;
        adsrParams.decay   = decay;
        adsrParams.sustain = sustain;
        adsrParams.release = release;
        adsr.setParameters (adsrParams);
    }

    void updateFilter(float cutoff, float resonance, int type)
    {
        filter.setCutoffFrequency(cutoff);
        filter.setResonance(resonance);

        switch (type)
        {
            case 0: filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass); break;
            case 1: filter.setType(juce::dsp::StateVariableTPTFilterType::highpass); break;
            case 2: filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass); break;
        }
    }

    void updateOscillators (int wave1, int wave2, float blendAmount)
    {
        osc1.setWaveform (wave1);
        osc2.setWaveform (wave2);
        blend = juce::jlimit(0.0f, 1.0f, blendAmount);
    }

    void updateOscOnOff(bool o1, bool o2)
    {
        osc1On = o1;
        osc2On = o2;
    }


///////////////////////////////////////////
private:
    Oscillator osc1, osc2;
    juce::AudioBuffer<float> tempBuffer1, tempBuffer2;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    juce::AudioBuffer<float> mixBuffer;

    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::dsp::ProcessSpec filterSpec;


    float baseFrequency = 0.0f;
    float level         = 0.0f;
    bool  isActive      = false;

    float gain       = 0.8f;
    float detune     = 0.0f;
    float pitchShift = 0.0f;
    float fmAmount   = 0.0f;

    float blend = 0.5f;
    bool osc1On = true;
    bool osc2On = true;
};


#endif //EFFEM_UNIT_SYNTHVOICE_H