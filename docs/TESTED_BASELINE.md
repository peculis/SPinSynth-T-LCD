# Tested baseline

## Confirmed production configuration

| Item | Baseline |
| --- | --- |
| Board | Teensy 4.0 at 600 MHz |
| USB type | Serial + MIDI + Audio |
| Audio outputs | USB Audio and PJRC Audio Shield Rev D2 simultaneously |
| Shield headphone volume | `0.85` |
| MIDI inputs | MIDI-DIN and USB MIDI |
| Host | Logic Pro on macOS |
| Display | KEYES DC 3.3 V LCD1602 I2C at `0x27` |
| I2C translation | None; direct 3.3 V bus |
| I2C pull-ups | Audio Shield 2.2 kΩ to 3.3 V |

## Validation results

- On 6-7 August 2026, the firmware tagged `V1.0` ran continuously for more than five hours without any failure or USB dropout.
- Throughout the tagged V1.0 endurance test, MIDI, USB Audio, Audio Shield audio, LCD/HMI, and heartbeat remained fully operational.
- The maximum reported CPU temperature during the final V1.0 validation was approximately 58.1 °C.
- Continuous production-hardware test exceeded five hours.
- MIDI-DIN, USB Audio, Audio Shield audio, LCD/HMI, and heartbeat remained operational.
- CPU temperature was 54.3 °C during the five-hour run.
- SDA and SCL measured approximately 3.25 V while the HMI operated.
- After temporary test code was removed, the cleaned production build completed a ten-minute CAT with all functions correct.
- CPU temperature during final CAT was 57.5 °C at approximately 28 °C ambient temperature.
- USB dropouts ceased after replacing the micro-USB cable and cleaning its connectors.

## V1.1 stereo reverb validation

PJRC `AudioEffectFreeverbStereo` was added as a global effect after the monophonic
synth engine. Two output mixers combine the original dry signal with Freeverb's
wet left and right outputs before routing the same stereo result to USB Audio and
the Audio Shield. MIDI CC 36 and the HMI `REVERB MIX` parameter control only the
dry/wet mixer gains; Freeverb continues processing at every mix setting.

The reverb was measured on Teensy 4.0 at 600 MHz using Teensy Audio Library 1.3
from Teensyduino 1.62.0. Room size and damping were both fixed at `0.5`.

| Test condition | Total Audio CPU | Reverb CPU |
| --- | ---: | ---: |
| Startup, default 0% wet | 5.98% | 3.59% |
| Notes playing, 0% wet | 6.18% | 3.76% |
| No notes, 50% wet | 6.26% | 3.76% |
| Notes playing, 50% wet | 6.86% | 4.35% |
| Notes playing, 100% wet | 6.25% | 3.75% |
| Maximum observed | 7.00% | 4.49% |

- CPU temperature remained between 57.5 °C and 58.1 °C during the measurements.
- Changing the dry/wet mix produced no meaningful CPU change; the small variation
  followed active versus silent audio blocks and normal scheduling variation.
- Stereo Freeverb uses approximately 51 KB of additional static RAM1 for its delay
  buffers. The compiled build retained 308,224 bytes free in RAM1 and 497,280 bytes
  free in RAM2.
- The temporary CPU diagnostic fields were removed after measurement. Normal uptime
  and temperature reporting remain enabled.

## Captured toolchain

| Component | Version |
| --- | --- |
| Teensy board package / Teensyduino | 1.62.0 |
| MIDI Library | 5.0.2 |
| LiquidCrystal I2C | 1.1.2, Frank de Brabander / Marco Schwartz |
| macOS | 15.7.3 |

Arduino IDE and Logic Pro application versions were not captured. They should be recorded before tagging a formal release.

## Regression checklist

- [x] Sketch compiles for Teensy 4.0 with Serial + MIDI + Audio.
- [x] Teensy enumerates as MIDI and Audio USB devices.
- [x] MIDI-DIN note-on and note-off play and stop notes cleanly.
- [x] USB Audio carries the synth signal on left and right channels.
- [x] Audio Shield headphones carry the synth signal.
- [x] USB Audio and Shield audio work simultaneously.
- [x] Master volume and filter controls respond.
- [x] Both ADSR envelopes respond.
- [x] Both encoders and push-buttons respond.
- [x] LCD initializes and updates from HMI/MIDI changes.
- [x] LCD I2C address `0x27` reports status `0`.
- [x] SGTL5000 address `0x0A` reports status `0`.
- [x] Heartbeat LED continues blinking.
- [x] CPU temperature and uptime continue reporting.
- [x] No new Teensy crash report is present.
- [x] Ten-minute CAT passed after production cleanup.

Repeat this checklist before merging future firmware or hardware changes.
