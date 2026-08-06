//----------------------------------------------------------------------------------
//SPinSynthAudio.h - Last update: 28 Jun 2026 - Started: 26 Jun 2026.
//----------------------------------------------------------------------------------
//28 Jun 2026: Implemented the interface between the SPinSynth synthesis engine
//and the Teensy 4.0 Audio Library.
//----------------------------------------------------------------------------------
//SPinSynthAudio renders the Oscillator, Filter and Amplifier into Teensy Audio
//blocks. These blocks can be sent to USB Audio or to the Audio Adapter using I2S.
//The SPinSynth synthesis engine remains independent from the Audio Library.
//----------------------------------------------------------------------------------

#ifndef SPIN_SYNTH_AUDIO_H
#define SPIN_SYNTH_AUDIO_H

#include <Arduino.h>
#include <AudioStream.h>

#include "Amplifier.h"
#include "Filter.h"
#include "Oscillator.h"
#include "SynthUtilities.h"

class SPinSynthAudio : public AudioStream {

public:
  SPinSynthAudio(Oscillator& oscillator, Filter& filter, Amplifier& amplifier);
  void begin();
  virtual void update(void);

private:
  void advanceControls();
  void renderNextBlock();
  int16_t convertToAudioSample(long sample);

  Oscillator& mOscillator;
  Filter& mFilter;
  Amplifier& mAmplifier;
  DoubleBuffer mOscillatorBuffer;
  DoubleBuffer mFilterBuffer;
  DoubleBuffer mAmplifierBuffer;

  static const int CONTROL_UPDATE_PERIOD_US = 250;
};

#endif
