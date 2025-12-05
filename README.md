# EFFEM UNIT
## Alisdair Chauvin 12/01/25

### Synthesizer built with JUCE using CMake. 

Options
- Choose between multiple waveforms with two oscillators
- Adjust pitch and detune settings
- Adjust envelopes and filters
- Visualize waveform

Usage
- Accepts MIDI input
- Create leads, pads, basses, & even drums
  - # Can you do it.....?
- Experiment with frequency modulation
  - f(t) = 440 Hz + (modulator_sample × FM_amount)
  - *Not yet implemented unfortunately*
- Usable as a standalone VST3 plugin or added audio plugin within digital audio workstations.

Citations:
- This project would not have been possible without JUCE and all of the tutorials provided 
  (https://juce.com/tutorials/tutorial_audio_thumbnail/)
- The Audio Programmer on YouTube (https://www.youtube.com/@TheAudioProgrammer) gave me the GitHub 
  repository via CLion, helped me code a large amount of the beginning code, and from there I adjusted 
  the UI and parameters to my preferences.
- The Coding Train helped me code additive synthesis (https://youtu.be/okfZRl4Xw-c?si=jrfowP0ozCGOjqEv)