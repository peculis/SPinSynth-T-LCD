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
