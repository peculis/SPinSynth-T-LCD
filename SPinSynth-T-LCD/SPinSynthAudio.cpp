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

void SPinSynthAudio::renderDiagnosticTone(audio_block_t* block) {
  static uint32_t diagnosticPhase = 0;
  const uint32_t diagnosticPhaseIncrement = uint32_t(440.0f * 4294967296.0f / AUDIO_SAMPLE_RATE_EXACT);

  for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++){
    block->data[i] = (diagnosticPhase & 0x80000000) ? 8000 : -8000;
    diagnosticPhase += diagnosticPhaseIncrement;
  }
}

void SPinSynthAudio::update(void) {
  audio_block_t* block = allocate();
  static int diagnosticBlocks = int((AUDIO_SAMPLE_RATE_EXACT * DIAGNOSTIC_TONE_SECONDS) / AUDIO_BLOCK_SAMPLES);

  if(block == NULL){
    return;
  }

  // Diagnostic mode 1 runs continuously so USB Audio can be configured in Logic.
  // Diagnostic mode 2 keeps the original startup-only tone, then returns to synth audio.

  if(DIAGNOSTIC_TONE_MODE == 1){
    renderDiagnosticTone(block);
    transmit(block);
    release(block);
    return;
  }
  else if(DIAGNOSTIC_TONE_MODE == 2 && diagnosticBlocks > 0){
    renderDiagnosticTone(block);
    diagnosticBlocks--;
    transmit(block);
    release(block);
    return;
  }

  renderNextBlock();

  for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++){
    block->data[i] = convertToAudioSample(mAmplifierBuffer.read());
  }

  transmit(block);
  release(block);
}
