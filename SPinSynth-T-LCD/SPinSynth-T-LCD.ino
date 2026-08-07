//----------------------------------------------------------------------------------
//SPinSynth-T-LCD - Monophonic Software Synthesizer Developed by Ricardo Peculis. 
//Last update: 7 August 2026 - Started: 22 Feb 2015
//This version runs on Teensy 4.0 with simultaneous USB Audio and PJRC Audio Shield Rev D2 output.
//The HMI uses a native 3.3 V KEYES I2C LCD 16x2 and two rotary encoders. MIDI-DIN and USB-MIDI
//remain available. See HARDWARE.md for the validated hardware configuration and its history.
//----------------------------------------------------------------------------------
//7 August 2026 - Began V1.1 development from the tagged V1.0 baseline. Added PJRC
//AudioEffectFreeverbStereo as a global effect with identical stereo output through USB Audio and
//the Audio Shield. MIDI CC 36 and the HMI REVERB MIX parameter control the dry/wet balance from
//0 to 100 percent. The default is fully dry to preserve the V1.0 startup sound. Freeverb room size
//and damping initially use conservative fixed values of 0.5.
//----------------------------------------------------------------------------------
//6 August 2026 - Established the new tested hardware and software baseline.
//
//Hardware changes:
//- Installed and validated a PJRC Audio Shield Rev D2. The synth now plays simultaneously through
//  USB Audio and the Shield headphone output. SGTL5000 headphone volume is initialized to 0.85.
//- Replaced the former 5 V LCD arrangement with a KEYES DC 3.3 V LCD1602 I2C module at address
//  0x27. It connects directly to Teensy pins 18 (SDA) and 19 (SCL), without a logic-level shifter.
//- The LCD has no local I2C pull-ups; the Audio Shield provides 2.2 kOhm pull-ups to 3.3 V. SDA
//  and SCL measured approximately 3.25 V while the HMI was operating.
//- Removed the HW-024/BSS138 level-shifter arrangement. Investigation of the earlier 5 V LCD
//  configuration revealed reversed HW-024 side assumptions and a possible back-powering path.
//- Replaced the intermittent micro-USB cable and cleaned the connectors with isopropyl alcohol.
//  This eliminated USB enumeration losses observed in macOS Audio MIDI Setup and Logic Pro.
//
//Software changes:
//- Added the SPinSynthAudio Teensy AudioStream interface and routed its mono synth output to both
//  channels of USB Audio and both channels of the Audio Shield I2S output.
//- Added the LCD/rotary-encoder HMI while preserving MIDI CC control, USB-MIDI and MIDI-DIN.
//- Corrected EnvelopeGenerator initialization and amplifier sustain behavior. The MIDI-DIN handler
//  ignores the controller's unsolicited CC 82 message so it cannot mute amplifier sustain; USB-MIDI
//  retains the complete CC mapping.
//- Corrected the duplicate PULSE parameter navigation while retaining the dedicated pulse-width edit.
//- Added CrashReport output, built-in LED heartbeat, startup I2C status checks, uptime and CPU
//  temperature reporting for operational fault diagnosis.
//- Removed the temporary continuous-tone, USB-only and HMI polling test paths after validation.
//
//Validation result:
//The production configuration completed a continuous test exceeding five hours with MIDI-DIN,
//USB Audio, Audio Shield audio, LCD/HMI and heartbeat operating normally. CPU temperature was
//54.3 degrees C. The complete hardware investigation and baseline are documented in HARDWARE.md.
//----------------------------------------------------------------------------------
//SPinSynth-T - Monophonic Software Synthesizer Developed by Ricardo Peculis. 
//Last update: 9 June 2026 - Started: 22 Feb 2015
//----------------------------------------------------------------------------------
//To compile this sketch, select Teensy 4.0 and USB Type Serial + MIDI + Audio.
//----------------------------------------------------------------------------------
//9 June 2026 - Today I am committing this sketch to GitHub as it is compiling and working as when I
//developed it in 2015. I had to reassemble the hardware in a breadboard because I damaged the original
//printed circuit board. It is working now. I tested all the functions, including MIDI and USB-MIDI.
//I used Arturia KEYLAB25 to test the SPinSynth-T (see the list of MIDI Controls below).
//It is important to observe that SPinSynth-T is not a finished product. It was used to experiment the
//development of a software synthesizer using Teensy 3.1 micro-computer. I developed my own software
//for the Oscillators, Filter, Amplifier, Envelope Generators and I did not make use of the Audio Library
//developed by Paul Stoffregen from PJRC. SPinSynth-T, however, uses his MIDI and USB-MIDI.
//----------------------------------------------------------------------------------
//26 April 2026 - Eleven years after the last change to this sketch, I decided to compile it again.
//The reason was to make sure it was still working to become a baseline for further improvements.
//I will include here a history for my own benefit.
//In the Arduino IDE, I selected the Teensy 3.2/3.1 board and USB Type: USB + MIDI (selected in Tools).
//To my surprise, or not, the sketch did not compile. The reasons beat me. There were too many errors
//and warnings. After a few days working on it, I fixed the errors and a few warnings (there are many
//warnings still there). After a successful compiling the sketch did not work as expected. The synth
//played with strange noises. After much investigation, I figured out that the anti-aliasing technique
//I implemented (polyblep with tables) was causing the problem. Initially I removed the anti-aliasing
//mechanism and the weird noise disappeared. 
//----------------------------------------------------------------------------------
//MIDI Controls from Arturia KEYLAB25 (11 Feb 2017)
//PitchBend   - CC#0  - Pitch Bend
//ModWheel   - CC#1  - Modulation Wheel: LFO Amount and Oscillator Frequency
//VOLUME Pot - CC#7  - Master Volume
//Bank 1 P1  - CC#74 - Filter Cutoff Frequency
//Bank 1 P2  - CC#71 - Filter Resonance
//Bank 1 P3  - CC#76 - Filter LFO Frequency
//Bank 1 P4  - CC#77 - Filter LFO Amount
//Bank 1 P5  - CC#93 - Filter LFO Waveshape: SINE, SAW, INVERTED-SAW, SQUARE/PULSE
//Bank 1 P6  - CC#18 - Mod Wheel Function: Filter CUTOFF, Filter Resonance, LFO_FILTER, LFO_OSCILLATOR
//Bank 1 P7  - CC#19 - Filter Envelope Level
//Bank 1 P8  - CC#16 - Oscillator LFO Rate
//Bank 1 P9  - CC#17 - Oscillator LFO Amount
//Bank 1 P10 - CC#91 - Oscillator LFO Waveshape: SINE, SAW, INVERTED-SAW, SQUARE/PULSE
//Bank 2 P1  - CC#35 - Portamento (Glide)
//Bank 2 P2  - CC#36 - Reverb Dry/Wet Mix
//Bank 2 P3  - CC#37 - SAW-X-TACTOR
//Bank 2 P4  - CC#38 - Oscillator PULSE-WIDTH
//Bank 2 P5  - CC#39 - Oscillator Sub-Factor Amount
//Bank 2 P6  - CC#40 - Oscillator Waveshape: SINE, SAW, SQUARE/PULSE, XTREME
//Bank 2 P7  - CC#41 - MASTER TUNE
//Bank 2 P8  - CC#42 - Oscillator SAW Amount
//Bank 2 P9  - CC#43 - Oscillator PULSE Amount
//Bank 2 P10 - CC#44 - Oscillator SUB Amount
//F1         - CC#73 - Filter Attack
//F2         - CC#75 - Filter Decay
//F3         - CC#79 - Filter Sustain
//F4         - CC#72 - Filter Release
//F5         - CC#80 - Amplifier Attack
//F6         - CC#81 - Amplifier Decay
//F7         - CC#82 - Amplifier Sustain
//F8         - CC#83 - Amplifier Release
//F8         - CC#92 - NOT USED
//----------------------------------------------------------------------------------
//23 Apr 2015: Fixed USB MIDI initialisation ( usbMIDI.begin(); was commented out ) 
//----------------------------------------------------------------------------------
//10 Apr 2015: Implemented Tuning LEDs.
//----------------------------------------------------------------------------------
//8 Apr 2015: I had a compilation scare today. This is a safety checkin to make sure 
//I did not lose any recent uncontrolled changes.
//----------------------------------------------------------------------------------
//4 Apr 2015: Implemented master Volume CC#7. Changed Filter Envelop Amount to
//Bank1 Pot P7 (Fader F9 Bank 1 is not used). Changed the default Filter CutOff 
//to max value (0.9, open filter).
//----------------------------------------------------------------------------------
//3 Apr 2015: Started SPinSynth-02 to remove classes and files that are not in use.
///Created SynthTables that contains a number of tables used for Waveform Generation, 
//Anti-Aliasing BLEP correction and MIDI Pitch to Frequency.
//Added Controls for Oscillator LF0 (Bank1: P8 Freq, P9 Amount, P10 Waveform.
//Implemented Master Tuning +/- OneTone - Bank2 P7, 63 = 440Hz.
//-----------------------------------------------------------------------
//2 Apr 2015: Implemented anti-aliasing Blep algorithm as a table. The Oscillator
//(see XtOscillator class) is now more efficient and I did not experience any crashes 
//as reported on 29 March.
//----------------------------------------------------------------------------------
//29 Mar 2015: Implemented Note ON/OFF event management to replicate the behaviour of
//monophonic keyboards where when two notes are pressed simultaneously the higher note
//will play and when the highest key is released the higher key immediately below
//that is still pressed will play. Implemented also the effect of Key Velocity applied to the
//amount of Envelope Amplitude Modulation and the amount of Envelope Filter Cutoff Modulation.
//With these I completed the basic functionality for the SPin Synth. There are improvements
//to be made but from now on is just "putting icing on the cake".
//-----------------------------------------------------------------------
//19 Mar 2015: USB MIDI is now working including Pitch Bend control
//Moved the Note ON LED to the EnvelopeGenerator Class.
//----------------------------------------------------------------------------------
//16 Mar 2015: Changed XTREME waveform to implement Detuned and SubOscillator
//Detuned detunes the a second SAW waveform by +/- 100 Cents (or one semi-tone)
//SubOscillator adds a third SAW waveform with frequency one octave below (P3 Bank2)
//or one fifth above (P5 Bank2). PULSE WIDTH is controlled by P4 Bank 2. The amount 
//of XTREME SAW, PULSE and SUB can be controlled separately by P8, P9 and P10 Bank 2.
//XTREME SAW Amount CC#42, XTREME FACTOR CC#37: P8 and P3 Bank2
//PULSE Amount CC#43, PULSE WIDTH CC#37: P9 and P4 Bank2
//SUB Amount CC#44, SUB FACTOR CC#39: P10 and P5 Bank2
//-----------------------------------------------------------------------
//15 Mar 2015: Implemented XTREME SAW modulated by two LFOs (see XtOscillator class). 
//Still requires tuning, and additional control, but it works.
//----------------------------------------------------------------------------------
//15 Mar 2015: Implemented the Low Frequency Oscillator (LFO) class and applied it to
//control the Oscillator frequency by setting LFO Rate (0.1 to 12.7Hz), amount (0.0 to 1.0)
//and LFO Waveform (SINE, SAW, INVERTED_SAW, SQUARE and PULSE). 
//I am now using the two Control Change (CC) Banks as following:
//Bank1: P1 Filter Cutoff, Pe Filter Resonance, P3 LFO Rate, P4 LFO Amount, P5 LFO Waveform.
//Modulation Wheel (CC#1) is controlling LFO1 Amount to control Oscillator Pitch Modulation.
//Bank1: P6 to P10 are not in use yet.
//Bank1 Slides: Filter ADSR, Amplifier ADSR and Filter ADSR Amount.
//Bank2: P2, P5, P7 AND P10 are not yet in use.
//Bank2: P1 Portamento (Glide), P8 SAW Amount, P9 SQUARE_PULSE Amount
//Bank2: P3 reserved (SAW) XFACTOR, P4 PULSEWIDTH
//----------------------------------------------------------------------------------
//13 Mar 2015: Resolved the "glitch" caused by high processing load. Initially 
//I thought I had improved performance by replacing floating point with fixed
//point calculation, but that was not the case, it got worse, and I reverted
//to floating point. Therefore, the Oscillator produces samples using floating point.
//Introduced a new waveshape option XTREME in addition to the existing ones. XTREME
//will implement all the features of paraphony (for the moment it just mixes SAW and
//SQUARE_PULSE.
//----------------------------------------------------------------------------------
//12 Mar 2015: I start here the final version of my synthesizer based on Teensy 3.1 
//microcontroller, now names as SPinSynth-01. This version starts from everything 
//I have developed so far as Synth-01-Teensy and it is already a fully operational 
//synthesizer with one VA Oscillator, VA Filter, VA Amplifier, Envelope Generator
//and MIDI interface. Now I will start introducing additional features.
//----------------------------------------------------------------------------------

#include "SynthUtilities.h"

#include "Oscillator.h"
// #include "XtOscillator.h"
#include "Filter.h"
#include  "Amplifier.h"
#include "SPinSynthAudio.h"
#include <Arduino.h>
#include <Audio.h>
#if defined(USB_AUDIO) || defined(USB_MIDI_AUDIO_SERIAL) || defined(USB_MIDI16_AUDIO_SERIAL)
#include <usb_audio.h>
#endif

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#include <Encoder.h>
#include <LiquidCrystal_I2C.h>
#include "SPinSynthHMI.h"

LiquidCrystal_I2C lcd (0x27, 16, 2);

Encoder knobLeft(2, 3);      // Pin 2 = Data Left and Pin 3 = Clock Left
const int buttonLeftPin = 4; // Pin 4 = Push Button Left
Encoder knobRight(5, 9);       // Pins moved away from the Audio Adapter SPI pins
const int buttonRightPin = 14;
SPinSynthHMI hmi(lcd, knobLeft, knobRight, buttonLeftPin, buttonRightPin);

enum ModWheelFunction {
   CUTOFF,
   RESONANCE,
   LFO_FILTER,
   LFO_OSCILLATOR
 };

 // XtOscillator vaOscillator;
 Oscillator vaOscillator;
 Filter vaFilter;
 Amplifier vaAmplifier;

 SPinSynthAudio spinSynthAudio(vaOscillator, vaFilter, vaAmplifier);
#if !defined(USB_AUDIO) && !defined(USB_MIDI_AUDIO_SERIAL) && !defined(USB_MIDI16_AUDIO_SERIAL)
#error "SPinSynth-T requires Tools > USB Type to include Audio, such as Serial + MIDI + Audio."
#endif
 AudioOutputUSB usbAudioOutput;
 AudioOutputI2S shieldAudioOutput;
 AudioControlSGTL5000 audioShield;
 AudioEffectFreeverbStereo reverb;
 AudioMixer4 reverbMixerLeft;
 AudioMixer4 reverbMixerRight;
 AudioConnection patchCordReverbInput(spinSynthAudio, 0, reverb, 0);
 AudioConnection patchCordDryLeft(spinSynthAudio, 0, reverbMixerLeft, 0);
 AudioConnection patchCordDryRight(spinSynthAudio, 0, reverbMixerRight, 0);
 AudioConnection patchCordWetLeft(reverb, 0, reverbMixerLeft, 1);
 AudioConnection patchCordWetRight(reverb, 1, reverbMixerRight, 1);
 AudioConnection patchCordUsbLeft(reverbMixerLeft, 0, usbAudioOutput, 0);
 AudioConnection patchCordUsbRight(reverbMixerRight, 0, usbAudioOutput, 1);
 AudioConnection patchCordShieldLeft(reverbMixerLeft, 0, shieldAudioOutput, 0);
 AudioConnection patchCordShieldRight(reverbMixerRight, 0, shieldAudioOutput, 1);
 
 const float MIDI_CC_MAX_VALUE = 127.0;
 const float CENTER_PULSE_WIDTH = 0.5;
 const float PULSE_WIDTH_MOD_DEPTH = 0.85;
 const float OSC_PULSE_WIDTH_CC_DIVISOR = 317.0;
 
 ModWheelFunction mModWheelFunction = LFO_OSCILLATOR;
 byte mModWheelValue = 0;
 
 // Global MIDI controls
 const byte CC_MOD_WHEEL = 1;
 const byte CC_MASTER_VOLUME = 7;
 const byte CC_MASTER_TUNE = 41;
 const byte CC_MOD_WHEEL_FUNCTION = 18;
 const byte CC_REVERB_MIX = 36;

 // Oscillator MIDI controls
 const byte CC_OSC_LFO_RATE = 16;
 const byte CC_OSC_LFO_AMOUNT = 17;
 const byte CC_OSC_LFO_WAVEFORM = 91;
 const byte CC_OSC_SAW_AMOUNT = 42;
 const byte CC_OSC_SAW_XFACTOR = 37;
 const byte CC_OSC_PULSE_AMOUNT = 43;
 const byte CC_OSC_PULSE_WIDTH = 38;
 const byte CC_OSC_SUB_AMOUNT = 44;
 const byte CC_OSC_SUB_FACTOR = 39;
 const byte CC_OSC_WAVEFORM = 40;
 const byte CC_PORTAMENTO = 35;

 // Filter MIDI controls
 const byte CC_FILTER_CUTOFF = 74;
 const byte CC_FILTER_RESONANCE = 71;
 const byte CC_FILTER_LFO_RATE = 76;
 const byte CC_FILTER_LFO_AMOUNT = 77;
 const byte CC_FILTER_LFO_WAVEFORM = 93;
 const byte CC_FILTER_ATTACK = 73;
 const byte CC_FILTER_DECAY = 75;
 const byte CC_FILTER_SUSTAIN = 79;
 const byte CC_FILTER_RELEASE = 72;
 const byte CC_FILTER_ENVELOPE_LEVEL = 19;

 // Amplifier MIDI controls
 const byte CC_AMP_ATTACK = 80;
 const byte CC_AMP_DECAY = 81;
 const byte CC_AMP_SUSTAIN = 82;
 const byte CC_AMP_RELEASE = 83;

 // LFO waveform value ranges
 const byte LFO_WAVE_SINE_MAX = 16;
 const byte LFO_WAVE_SAW_MAX = 32;
 const byte LFO_WAVE_INVERTED_SAW_MAX = 48;
 const byte LFO_WAVE_SQUARE_MAX = 64;

 // Oscillator waveform value ranges
 const byte OSC_WAVE_SINE_MAX = 20;
 const byte OSC_WAVE_SAW_MAX = 42;
 const byte OSC_WAVE_SQUARE_MAX = 62;
 const byte OSC_WAVE_PULSE_MAX = 119;

 // Mod wheel function value ranges
 const byte MOD_WHEEL_CUTOFF_MAX = 20;
 const byte MOD_WHEEL_RESONANCE_MAX = 42;
 const byte MOD_WHEEL_FILTER_LFO_MAX = 62;

 //---------------- MIDI Callbacks -------------------------------
 bool activeMidiNotes[16][128] = {};
 uint16_t activeMidiNoteCount = 0;

 void HandleNoteOff(byte channel, byte pitch, byte velocity);

 void ApplyModWheel(byte value) {
  switch(mModWheelFunction){
    case CUTOFF:
      vaFilter.setModCutoff(value);
      break;
    case RESONANCE:
      vaFilter.setModResonance(value);
      break;
    case LFO_FILTER:
      vaFilter.setLFOamount(value);
      break;
    case LFO_OSCILLATOR:
      vaOscillator.setLFOamount(value);
      break;
  }
 }

 void RestoreModWheelDestinations() {
  vaFilter.setModCutoff(0);
  vaFilter.setModResonance(0);
  vaFilter.setLFOamount(
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_LEVEL));
  vaOscillator.setLFOamount(
    hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_LEVEL));
 }

 int filterEnvelopeAmountToMidi(int amount) {
  if(amount <= 0){
    return 63 - int((long(-amount) * 63L + 50L) / 100L);
  }
  return 64 + int((long(amount) * 63L + 50L) / 100L);
 }

 int filterEnvelopeAmountFromMidi(byte value) {
  if(value <= 63){
    return int(100L * (int(value) - 63) / 63L);
  }
  return int(100L * (int(value) - 64) / 63L);
 }

 void ApplySynthParameter(SPinSynthHMI::ParameterId parameter, int value) {
  switch(parameter){
    case SPinSynthHMI::PARAM_MASTER_VOLUME:
      vaAmplifier.setVolume(value);
      break;
    case SPinSynthHMI::PARAM_MASTER_TUNING:
      // HMI stores hundredths of a whole tone; one whole tone is 200 cents.
      vaOscillator.setTuningCents(2.0 * float(value));
      break;
    case SPinSynthHMI::PARAM_PORTAMENTO:
      vaOscillator.setPortamentoTimeMs(value);
      break;
    case SPinSynthHMI::PARAM_MOD_WHEEL_FUNCTION:
      RestoreModWheelDestinations();
      switch(value){
        case 0:
          mModWheelFunction = CUTOFF;
          break;
        case 1:
          mModWheelFunction = RESONANCE;
          break;
        case 2:
          mModWheelFunction = LFO_FILTER;
          break;
        case 3:
        default:
          mModWheelFunction = LFO_OSCILLATOR;
          break;
      }
      ApplyModWheel(mModWheelValue);
      break;
    case SPinSynthHMI::PARAM_REVERB_MIX:{
      const float wetGain = float(value) / 100.0f;
      const float dryGain = 1.0f - wetGain;
      reverbMixerLeft.gain(0, dryGain);
      reverbMixerLeft.gain(1, wetGain);
      reverbMixerRight.gain(0, dryGain);
      reverbMixerRight.gain(1, wetGain);
      break;
    }
    case SPinSynthHMI::PARAM_OSC_WAVEFORM:
      switch(value){
        case 0:
          vaOscillator.setWaveform(SINE);
          break;
        case 1:
          vaOscillator.setWaveform(SAW);
          break;
        case 2:
          vaOscillator.setWaveform(SQUARE_PULSE);
          vaOscillator.setPulseWidth(0.5);
          break;
        case 3:
          vaOscillator.setWaveform(SQUARE_PULSE);
          vaOscillator.setPulseWidth(
            float(hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_PULSE_WIDTH)) / 100.0);
          break;
        case 4:
          vaOscillator.setWaveform(XTREME);
          ApplySynthParameter(
            SPinSynthHMI::PARAM_X_SAW_LEVEL,
            hmi.getParameterValue(SPinSynthHMI::PARAM_X_SAW_LEVEL));
          ApplySynthParameter(
            SPinSynthHMI::PARAM_X_DETUNE,
            hmi.getParameterValue(SPinSynthHMI::PARAM_X_DETUNE));
          ApplySynthParameter(
            SPinSynthHMI::PARAM_X_PULSE_LEVEL,
            hmi.getParameterValue(SPinSynthHMI::PARAM_X_PULSE_LEVEL));
          ApplySynthParameter(
            SPinSynthHMI::PARAM_X_PULSE_WIDTH,
            hmi.getParameterValue(SPinSynthHMI::PARAM_X_PULSE_WIDTH));
          ApplySynthParameter(
            SPinSynthHMI::PARAM_X_SUB_LEVEL,
            hmi.getParameterValue(SPinSynthHMI::PARAM_X_SUB_LEVEL));
          ApplySynthParameter(
            SPinSynthHMI::PARAM_X_SUB_FACTOR,
            hmi.getParameterValue(SPinSynthHMI::PARAM_X_SUB_FACTOR));
          break;
      }
      break;
    case SPinSynthHMI::PARAM_OSC_PULSE_WIDTH:
      if(hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_WAVEFORM) == 3){
        vaOscillator.setPulseWidth(float(value) / 100.0);
      }
      break;
    case SPinSynthHMI::PARAM_X_SAW_LEVEL:
      vaOscillator.setSawAmount(float(value) / MIDI_CC_MAX_VALUE);
      break;
    case SPinSynthHMI::PARAM_X_DETUNE:
      vaOscillator.setSawXfactor(float(value));
      break;
    case SPinSynthHMI::PARAM_X_PULSE_LEVEL:
      vaOscillator.setPulseAmount(float(value) / MIDI_CC_MAX_VALUE);
      break;
    case SPinSynthHMI::PARAM_X_PULSE_WIDTH:
      if(hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_WAVEFORM) == 4){
        vaOscillator.setPulseWidth(float(value) / 100.0);
      }
      break;
    case SPinSynthHMI::PARAM_X_SUB_LEVEL:
      vaOscillator.setSubAmount(float(value) / MIDI_CC_MAX_VALUE);
      break;
    case SPinSynthHMI::PARAM_X_SUB_FACTOR:
      vaOscillator.setSubFactor(float(value) / 100.0);
      break;
    case SPinSynthHMI::PARAM_OSC_LFO_WAVEFORM:
      switch(value){
        case 0:
          vaOscillator.setLFOwaveform(LFO_SINE);
          break;
        case 1:
          vaOscillator.setLFOwaveform(LFO_SAW);
          break;
        case 2:
          vaOscillator.setLFOwaveform(LFO_INVERTED_SAW);
          break;
        case 3:
          vaOscillator.setLFOwaveform(LFO_SQUARE_PULSE);
          vaOscillator.setLFOpulsewidth(CENTER_PULSE_WIDTH);
          break;
        case 4:
          vaOscillator.setLFOwaveform(LFO_SQUARE_PULSE);
          vaOscillator.setLFOpulsewidth(
            float(hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_PULSE_WIDTH)) / 100.0);
          break;
      }
      break;
    case SPinSynthHMI::PARAM_OSC_LFO_PULSE_WIDTH:
      if(hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_WAVEFORM) == 4){
        vaOscillator.setLFOpulsewidth(float(value) / 100.0);
      }
      break;
    case SPinSynthHMI::PARAM_OSC_LFO_FREQUENCY:
      vaOscillator.setLFOrate(value);
      break;
    case SPinSynthHMI::PARAM_OSC_LFO_LEVEL:
      vaOscillator.setLFOamount(value);
      break;
    case SPinSynthHMI::PARAM_AMP_ATTACK:
      vaAmplifier.setAttack(value);
      break;
    case SPinSynthHMI::PARAM_AMP_DECAY:
      vaAmplifier.setDecay(value);
      break;
    case SPinSynthHMI::PARAM_AMP_SUSTAIN:
      vaAmplifier.setSustain(value);
      break;
    case SPinSynthHMI::PARAM_AMP_RELEASE:
      vaAmplifier.setRelease(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_CUTOFF:
      vaFilter.setDialCutoff(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_RESONANCE:
      vaFilter.setDialResonance(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_ATTACK:
      vaFilter.setAttack(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_DECAY:
      vaFilter.setDecay(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_SUSTAIN:
      vaFilter.setSustain(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_RELEASE:
      vaFilter.setRelease(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_ENVELOPE_AMOUNT:
      vaFilter.setEnvelopeLevel(filterEnvelopeAmountToMidi(value));
      break;
    case SPinSynthHMI::PARAM_FILTER_LFO_WAVEFORM:
      switch(value){
        case 0:
          vaFilter.setLFOwaveform(LFO_SINE);
          break;
        case 1:
          vaFilter.setLFOwaveform(LFO_SAW);
          break;
        case 2:
          vaFilter.setLFOwaveform(LFO_INVERTED_SAW);
          break;
        case 3:
          vaFilter.setLFOwaveform(LFO_SQUARE_PULSE);
          vaFilter.setLFOpulsewidth(CENTER_PULSE_WIDTH);
          break;
        case 4:
          vaFilter.setLFOwaveform(LFO_SQUARE_PULSE);
          vaFilter.setLFOpulsewidth(
            float(hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_PULSE_WIDTH)) / 100.0);
          break;
      }
      break;
    case SPinSynthHMI::PARAM_FILTER_LFO_PULSE_WIDTH:
      if(hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_WAVEFORM) == 4){
        vaFilter.setLFOpulsewidth(float(value) / 100.0);
      }
      break;
    case SPinSynthHMI::PARAM_FILTER_LFO_FREQUENCY:
      vaFilter.setLFOrate(value);
      break;
    case SPinSynthHMI::PARAM_FILTER_LFO_LEVEL:
      vaFilter.setLFOamount(value);
      break;
    default:
      break;
  }
 }

 void HandleHMIParameterChange(SPinSynthHMI::ParameterId parameter, int value) {
  ApplySynthParameter(parameter, value);
 }

 void SetSynthParameter(SPinSynthHMI::ParameterId parameter, int value) {
  hmi.setParameterValue(parameter, value);
  ApplySynthParameter(parameter, hmi.getParameterValue(parameter));
 }

 int masterTuneFromMidi(byte value) {
  if(value <= 63){
    return int(100L * (int(value) - 63) / 63L);
  }
  return int(100L * (int(value) - 63) / 64L);
 }

 int oscillatorWaveformFromMidi(byte value) {
  if(value <= OSC_WAVE_SINE_MAX){
    return 0;
  }
  if(value <= OSC_WAVE_SAW_MAX){
    return 1;
  }
  if(value <= OSC_WAVE_SQUARE_MAX){
    return 2;
  }
  if(value <= OSC_WAVE_PULSE_MAX){
    return 3;
  }
  return 4;
 }

 int pulseDutyFromWaveformMidi(byte value) {
  const float pulsePosition = float(value - 62) / 62.0;
  const float pulseWidth = CENTER_PULSE_WIDTH *
                           (1.0 - PULSE_WIDTH_MOD_DEPTH * pulsePosition);
  return constrain(int(100.0 * pulseWidth + 0.5), 10, 50);
 }

 int pulseDutyFromMidi(byte value) {
  const float pulseWidth = CENTER_PULSE_WIDTH -
                           float(value) / OSC_PULSE_WIDTH_CC_DIVISOR;
  return constrain(int(100.0 * pulseWidth + 0.5), 10, 50);
 }

 int detuneFromMidi(byte value) {
  if(value <= 63){
    return int(100L * (int(value) - 63) / 63L);
  }
  return int(100L * (int(value) - 63) / 64L);
 }

 int subFactorFromMidi(byte value) {
  return 50 + int((100L * value + 63L) / 127L);
 }

 int lfoWaveformFromMidi(byte value) {
  if(value <= LFO_WAVE_SINE_MAX){
    return 0;
  }
  if(value <= LFO_WAVE_SAW_MAX){
    return 1;
  }
  if(value <= LFO_WAVE_INVERTED_SAW_MAX){
    return 2;
  }
  if(value <= LFO_WAVE_SQUARE_MAX){
    return 3;
  }
  return 4;
 }

 int lfoPulseDutyFromMidi(byte value) {
  const float pulsePosition = float(value - LFO_WAVE_SQUARE_MAX) /
                              float(LFO_WAVE_SQUARE_MAX);
  const float pulseWidth = CENTER_PULSE_WIDTH *
                           (1.0 - PULSE_WIDTH_MOD_DEPTH * pulsePosition);
  return constrain(int(100.0 * pulseWidth + 0.5), 10, 50);
 }

 void HandleNoteOn(byte channel, byte pitch, byte velocity) {
  // MIDI permits Note On with velocity zero as an alternative Note Off.
  if(velocity == 0){
    HandleNoteOff(channel, pitch, velocity);
    return;
  }

  const byte channelIndex = constrain(channel, 1, 16) - 1;
  if(activeMidiNotes[channelIndex][pitch]){
    return;
  }
  activeMidiNotes[channelIndex][pitch] = true;
  activeMidiNoteCount++;
  digitalWrite(LED_BUILTIN, HIGH);

  vaOscillator.noteON(int(pitch), int(velocity));
  vaFilter.trigger();
  vaFilter.noteON(int(pitch), int(velocity));
  vaAmplifier.trigger();
  vaAmplifier.noteON(int(pitch), int(velocity));
}

void HandleNoteOff(byte channel, byte pitch, byte velocity){
  const byte channelIndex = constrain(channel, 1, 16) - 1;
  if(!activeMidiNotes[channelIndex][pitch]){
    return;
  }
  activeMidiNotes[channelIndex][pitch] = false;
  if(activeMidiNoteCount > 0){
    activeMidiNoteCount--;
  }
  if(activeMidiNoteCount == 0){
    digitalWrite(LED_BUILTIN, LOW);
  }

  vaOscillator.noteOFF(int(pitch), int(velocity));
  vaFilter.noteOFF(int(pitch), int(velocity));
  vaAmplifier.noteOFF(int(pitch), int(velocity));
}

int modWheelFunctionFromMidi(byte value) {
  if(value <= MOD_WHEEL_CUTOFF_MAX){
    return 0;
  }
  if(value <= MOD_WHEEL_RESONANCE_MAX){
    return 1;
  }
  if(value <= MOD_WHEEL_FILTER_LFO_MAX){
    return 2;
  }
  return 3;
}

//ControlChange sends the numeric values of potentiometers and these are associated with
//a Control like Filter Frequency and Resonance, and Envelope Parameters (ADSR).
void HandleControlChange(byte channel, byte number, byte value) { 
  switch (number){
    case CC_MOD_WHEEL:
      mModWheelValue = value;
      ApplyModWheel(value);
      break;
    case CC_MASTER_TUNE: //Master Tuning
      SetSynthParameter(
        SPinSynthHMI::PARAM_MASTER_TUNING,
        masterTuneFromMidi(value));
      break;
    case CC_MOD_WHEEL_FUNCTION: // Selects the destination controlled by the Mod Wheel.
      SetSynthParameter(
        SPinSynthHMI::PARAM_MOD_WHEEL_FUNCTION,
        modWheelFunctionFromMidi(value));
      break;
    case CC_REVERB_MIX:
      SetSynthParameter(
        SPinSynthHMI::PARAM_REVERB_MIX,
        int((100L * long(value) + 63L) / 127L));
      break;
    case CC_OSC_LFO_RATE:
      SetSynthParameter(SPinSynthHMI::PARAM_OSC_LFO_FREQUENCY, value);
      break;
    case CC_OSC_LFO_AMOUNT:
      SetSynthParameter(SPinSynthHMI::PARAM_OSC_LFO_LEVEL, value);
      break;
    case CC_OSC_LFO_WAVEFORM:
      SetSynthParameter(
        SPinSynthHMI::PARAM_OSC_LFO_WAVEFORM,
        lfoWaveformFromMidi(value));
      if(value > LFO_WAVE_SQUARE_MAX){
        SetSynthParameter(
          SPinSynthHMI::PARAM_OSC_LFO_PULSE_WIDTH,
          lfoPulseDutyFromMidi(value));
      }
      break;
    case CC_OSC_SAW_AMOUNT:
      SetSynthParameter(SPinSynthHMI::PARAM_X_SAW_LEVEL, value);
      break;
     case CC_OSC_SAW_XFACTOR:
      SetSynthParameter(
        SPinSynthHMI::PARAM_X_DETUNE,
        detuneFromMidi(value));
      break;
    case CC_OSC_PULSE_AMOUNT:
      SetSynthParameter(SPinSynthHMI::PARAM_X_PULSE_LEVEL, value);
      break;
    case CC_OSC_PULSE_WIDTH:
      SetSynthParameter(
        hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_WAVEFORM) == 4
          ? SPinSynthHMI::PARAM_X_PULSE_WIDTH
          : SPinSynthHMI::PARAM_OSC_PULSE_WIDTH,
        pulseDutyFromMidi(value));
      break;
    case CC_OSC_SUB_AMOUNT:
      SetSynthParameter(SPinSynthHMI::PARAM_X_SUB_LEVEL, value);
      break;
    case CC_OSC_SUB_FACTOR:
      SetSynthParameter(
        SPinSynthHMI::PARAM_X_SUB_FACTOR,
        subFactorFromMidi(value));
      break;
    case CC_FILTER_CUTOFF:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_CUTOFF, value);
      break;
    case CC_FILTER_RESONANCE:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_RESONANCE, value);
      break;
    case CC_FILTER_LFO_RATE:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_LFO_FREQUENCY, value);
      break;
    case CC_FILTER_LFO_AMOUNT:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_LFO_LEVEL, value);
      break;
    case CC_FILTER_LFO_WAVEFORM:
      SetSynthParameter(
        SPinSynthHMI::PARAM_FILTER_LFO_WAVEFORM,
        lfoWaveformFromMidi(value));
      if(value > LFO_WAVE_SQUARE_MAX){
        SetSynthParameter(
          SPinSynthHMI::PARAM_FILTER_LFO_PULSE_WIDTH,
          lfoPulseDutyFromMidi(value));
      }
      break;
    case CC_FILTER_ATTACK:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_ATTACK, value);
      break;
    case CC_FILTER_DECAY:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_DECAY, value);
      break;
    case CC_FILTER_SUSTAIN:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_SUSTAIN, value);
      break;
    case CC_FILTER_RELEASE:
      SetSynthParameter(SPinSynthHMI::PARAM_FILTER_RELEASE, value);
      break;
    case CC_FILTER_ENVELOPE_LEVEL:
      SetSynthParameter(
        SPinSynthHMI::PARAM_FILTER_ENVELOPE_AMOUNT,
        filterEnvelopeAmountFromMidi(value));
      break;
    case CC_AMP_ATTACK:
      SetSynthParameter(SPinSynthHMI::PARAM_AMP_ATTACK, value);
      break;
    case CC_AMP_DECAY:
      SetSynthParameter(SPinSynthHMI::PARAM_AMP_DECAY, value);
      break;
    case CC_AMP_SUSTAIN:
      SetSynthParameter(SPinSynthHMI::PARAM_AMP_SUSTAIN, value);
      break;
    case CC_AMP_RELEASE:
      SetSynthParameter(SPinSynthHMI::PARAM_AMP_RELEASE, value);
      break;
    case CC_MASTER_VOLUME:
      SetSynthParameter(SPinSynthHMI::PARAM_MASTER_VOLUME, value);
      break;
    case CC_OSC_WAVEFORM:
      SetSynthParameter(
        SPinSynthHMI::PARAM_OSC_WAVEFORM,
        oscillatorWaveformFromMidi(value));
      if(value >= 63 && value <= 119){
        SetSynthParameter(
          SPinSynthHMI::PARAM_OSC_PULSE_WIDTH,
          pulseDutyFromWaveformMidi(value));
      }
      break;
    case CC_PORTAMENTO:
      SetSynthParameter(SPinSynthHMI::PARAM_PORTAMENTO, 2 * int(value));
      break;  
    default:
      break;
  }
}

void HandlePitchBend(byte channel, int bend) { 
  vaOscillator.setPitchBend(bend);
}

// This DIN controller sends CC 82 with value 0 without the user requesting an
// amplifier-sustain change. Ignore only that DIN message so it cannot mute the
// synth. USB MIDI keeps the complete CC mapping, including CC 82 automation.
void HandleDinControlChange(byte channel, byte number, byte value) {
  if(number == CC_AMP_SUSTAIN){
    return;
  }
  HandleControlChange(channel, number, value);
}
  
//----------------- End MIDI Callbacks -------------------------

void setup() {
  Serial.begin(9600);
  delay(1000);
  if(CrashReport){
    Serial.println("Previous Teensy crash report:");
    Serial.print(CrashReport);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
#if !defined(__IMXRT1062__)
  pinMode(LED, OUTPUT);
  pinMode(TUNED, OUTPUT);
  pinMode(FLAT, OUTPUT);
  pinMode(SHARP, OUTPUT);
#endif

  // The dry synth and stereo Freeverb output are mixed and sent simultaneously
  // to USB Audio and to the SGTL5000 Audio Shield over I2S.
  AudioMemory(32);
  for(int channel = 0; channel < 4; channel++){
    reverbMixerLeft.gain(channel, 0.0f);
    reverbMixerRight.gain(channel, 0.0f);
  }
  reverb.roomsize(0.5f);
  reverb.damping(0.5f);
  spinSynthAudio.begin();
  const bool audioShieldEnabled = audioShield.enable();
  const bool audioShieldVolumeSet = audioShield.volume(0.85);
  const bool audioShieldHeadphoneSelected =
    audioShield.headphoneSelect(AUDIO_HEADPHONE_DAC);
  const bool audioShieldHeadphoneUnmuted = audioShield.unmuteHeadphone();
  Serial.print("Audio Shield enable: ");
  Serial.print(audioShieldEnabled ? "OK" : "FAILED");
  Serial.print(", volume: ");
  Serial.print(audioShieldVolumeSet ? "OK" : "FAILED");
  Serial.print(", headphone DAC: ");
  Serial.print(audioShieldHeadphoneSelected ? "OK" : "FAILED");
  Serial.print(", unmute: ");
  Serial.println(audioShieldHeadphoneUnmuted ? "OK" : "FAILED");

  //MIDI Setup
  // Initiate MIDI communications, listen to all channels
  // MIDI_CREATE_DEFAULT_INSTANCE uses Serial1 on Teensy. This keeps the
  // 31,250-baud DIN interface active alongside the independent USB MIDI port.
  MIDI.begin(MIDI_CHANNEL_OMNI);    
  MIDI.setHandleNoteOn(HandleNoteOn);
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setHandleControlChange(HandleDinControlChange);
  MIDI.setHandlePitchBend(HandlePitchBend); 

#if defined(MIDI_INTERFACE)
  usbMIDI.begin();    
  usbMIDI.setHandleNoteOn(HandleNoteOn);
  usbMIDI.setHandleNoteOff(HandleNoteOff);
  usbMIDI.setHandleControlChange(HandleControlChange);
  usbMIDI.setHandlePitchChange(HandlePitchBend); 
#endif

  hmi.setParameterChangeHandler(HandleHMIParameterChange);
  hmi.begin(true);

  Wire.beginTransmission(0x27);
  const uint8_t lcdI2cStatus = Wire.endTransmission();
  Serial.print("LCD I2C 0x27 status: ");
  Serial.println(lcdI2cStatus);

  Wire.beginTransmission(0x0A);
  const uint8_t audioShieldI2cStatus = Wire.endTransmission();
  Serial.print("Audio Shield I2C 0x0A status: ");
  Serial.println(audioShieldI2cStatus);

  ApplySynthParameter(
    SPinSynthHMI::PARAM_MASTER_VOLUME,
    hmi.getParameterValue(SPinSynthHMI::PARAM_MASTER_VOLUME));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_MASTER_TUNING,
    hmi.getParameterValue(SPinSynthHMI::PARAM_MASTER_TUNING));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_PORTAMENTO,
    hmi.getParameterValue(SPinSynthHMI::PARAM_PORTAMENTO));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_MOD_WHEEL_FUNCTION,
    hmi.getParameterValue(SPinSynthHMI::PARAM_MOD_WHEEL_FUNCTION));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_REVERB_MIX,
    hmi.getParameterValue(SPinSynthHMI::PARAM_REVERB_MIX));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_OSC_WAVEFORM,
    hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_WAVEFORM));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_OSC_LFO_WAVEFORM,
    hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_WAVEFORM));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_OSC_LFO_PULSE_WIDTH,
    hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_PULSE_WIDTH));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_OSC_LFO_FREQUENCY,
    hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_FREQUENCY));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_OSC_LFO_LEVEL,
    hmi.getParameterValue(SPinSynthHMI::PARAM_OSC_LFO_LEVEL));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_AMP_ATTACK,
    hmi.getParameterValue(SPinSynthHMI::PARAM_AMP_ATTACK));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_AMP_DECAY,
    hmi.getParameterValue(SPinSynthHMI::PARAM_AMP_DECAY));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_AMP_SUSTAIN,
    hmi.getParameterValue(SPinSynthHMI::PARAM_AMP_SUSTAIN));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_AMP_RELEASE,
    hmi.getParameterValue(SPinSynthHMI::PARAM_AMP_RELEASE));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_CUTOFF,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_CUTOFF));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_RESONANCE,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_RESONANCE));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_ATTACK,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_ATTACK));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_DECAY,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_DECAY));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_SUSTAIN,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_SUSTAIN));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_RELEASE,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_RELEASE));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_ENVELOPE_AMOUNT,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_ENVELOPE_AMOUNT));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_LFO_WAVEFORM,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_WAVEFORM));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_LFO_PULSE_WIDTH,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_PULSE_WIDTH));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_LFO_FREQUENCY,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_FREQUENCY));
  ApplySynthParameter(
    SPinSynthHMI::PARAM_FILTER_LFO_LEVEL,
    hmi.getParameterValue(SPinSynthHMI::PARAM_FILTER_LFO_LEVEL));
}

void loop(){
  static unsigned long lastHeartbeatMs = 0;
  static unsigned long lastTemperatureMs = 0;
  static bool heartbeatState = false;
  const unsigned long nowMs = millis();

  if(nowMs - lastHeartbeatMs >= 500){
    lastHeartbeatMs = nowMs;
    heartbeatState = !heartbeatState;
    digitalWrite(LED_BUILTIN, heartbeatState ? HIGH : LOW);
  }

  if(nowMs - lastTemperatureMs >= 5000){
    lastTemperatureMs = nowMs;
    Serial.print("Uptime: ");
    Serial.print(nowMs / 1000);
    Serial.print(" s, Temp: ");
    Serial.print(tempmonGetTemp(), 1);
    Serial.println(" C");
  }

  MIDI.read();
#if defined(MIDI_INTERFACE)
  usbMIDI.read();
#endif
  hmi.update();
 }
