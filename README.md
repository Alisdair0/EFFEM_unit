# EFFEM UNIT
## Alisdair Chauvin 12/01/25

### Synthesizer built with JUCE using CMake. 

Options
- Choose between multiple waveforms
- Adjust pitch and detune settings
- Adjust envelopes and filters
- Visualize sound design in real time

Usage
- Accepts MIDI input
- Experiment with frequency modulation
  - f(t) = 440 Hz + (modulator_sample × FM_amount)
- Usable as a standalone VST3 plugin or added audio plugin within digital audio workstations.

Citations:
- The Audio Programmer on YouTube (https://www.youtube.com/@TheAudioProgrammer) helped me code
  a large amount of the beginning code, and from there I adjusted the UI and parameters to my preferences.