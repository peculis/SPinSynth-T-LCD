//----------------------------------------------------------------------------------
//SPinSynthAudio.cpp - Last update: 28 Jun 2026 - Started: 26 Jun 2026.
//----------------------------------------------------------------------------------
//28 Jun 2026: Implemented Teensy Audio block generation and the control update
//timing required by the SPinSynth synthesis engine.
//----------------------------------------------------------------------------------
//SPinSynthAudio runs the Oscillator, Filter and Amplifier and converts their
//output into 16 bit Teensy Audio blocks for USB Audio or the Audio Adapter.
//----------------------------------------------------------------------------------

#include "SPinSynthAudio.h"

SPinSynthAudio::SPinSynthAudio(Oscillator& oscillator, Filter& filter, Amplifier& amplifier):
  AudioStream(0, NULL),
  mOscillator(oscillator),
  mFilter(filter),
  mAmplifier(amplifier) {
}

void SPinSynthAudio::begin() {
  mOscillator.setSampleRate(AUDIO_SAMPLE_RATE_EXACT);
  const float blockTimeSeconds = float(AUDIO_BLOCK_SAMPLES) / AUDIO_SAMPLE_RATE_EXACT;
  const int controlUpdates = int(
    (1000000.0f * blockTimeSeconds / float(CONTROL_UPDATE_PERIOD_US)) + 0.5f);
  mOscillator.setControlUpdateRate(blockTimeSeconds / float(controlUpdates));
  mOscillatorBuffer.start(AUDIO_BLOCK_SAMPLES);
  mFilterBuffer.start(AUDIO_BLOCK_SAMPLES);
  mAmplifierBuffer.start(AUDIO_BLOCK_SAMPLES);
}

void SPinSynthAudio::advanceControls() {
  const float blockTimeUs = 1000000.0f * float(AUDIO_BLOCK_SAMPLES) / AUDIO_SAMPLE_RATE_EXACT;
  const int controlUpdates = int((blockTimeUs / float(CONTROL_UPDATE_PERIOD_US)) + 0.5f);

  for(int i = 0; i < controlUpdates; i++){
    mOscillator.update();
    mFilter.update();
    mAmplifier.update();
  }
}

void SPinSynthAudio::renderNextBlock() {
  advanceControls();

  if(mOscillatorBuffer.isReadyToWrite()){
    mOscillator.updateBuffer(mOscillatorBuffer);
    mOscillatorBuffer.swapBuffer();
  }

  if(mFilterBuffer.isReadyToWrite()){
    mFilter.updateBuffer(mOscillatorBuffer, mFilterBuffer);
    mFilterBuffer.swapBuffer();
  }

  if(mAmplifierBuffer.isReadyToWrite()){
    mAmplifier.updateBuffer(mFilterBuffer, mAmplifierBuffer);
    mAmplifierBuffer.swapBuffer();
  }
}

int16_t SPinSynthAudio::convertToAudioSample(long sample) {
  long audioSample = sample;

  if(audioSample > 32767){
    audioSample = 32767;
  }
  else if(audioSample < -32768){
    audioSample = -32768;
  }

  return int16_t(audioSample);
}

void SPinSynthAudio::update(void) {
  audio_block_t* block = allocate();

  if(block == NULL){
    return;
  }

  renderNextBlock();

  for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++){
    block->data[i] = convertToAudioSample(mAmplifierBuffer.read());
  }

  transmit(block);
  release(block);
}
