# SPinSynth-T-LCD

SPinSynth-T-LCD is a monophonic software synthesizer for Teensy 4.0. It combines Ricardo Peculis's custom synthesis engine with the PJRC Teensy Audio Library, MIDI-DIN, USB MIDI, simultaneous USB/Audio-Shield audio, and a local LCD/rotary-encoder HMI.

> **Project status:** experimental, working hardware baseline. This is not a finished product.

## Project history and credits

Ricardo Peculis began SPinSynth in February 2015 and developed the original SPinSynth-T monophonic synthesis engine for Teensy 3.1. Its oscillator, filter, amplifier, ADSR envelopes, LFOs and control logic are his custom implementation. The original instrument used the Teensy DAC and MIDI controls.

In 2026, SPinSynth-T-LCD brought that engine to Teensy 4.0. The `SPinSynthAudio` adapter integrates it with PJRC AudioStream and 16-bit audio output; the `SPinSynthHMI` component adds the LCD and two rotary encoders. The validated V1.0 baseline added simultaneous USB Audio and Audio Shield output. V1.1 adds stereo Freeverb, controlled from MIDI and the HMI.

This work was developed through Ricardo's hardware assembly, musical evaluation and Compile, Apply, Test (CAT) direction, in collaboration with **OpenAI Codex** for source analysis, implementation assistance, debugging, build checks and documentation. Physical acceptance and reported hardware measurements came from Ricardo's testing. The documented refinements include envelope initialization and sustain fixes, DIN CC82 handling, pulse-width navigation, startup diagnostics and the stereo audio routing. They do not establish a general DSP speedup claim.

Acknowledgements:

- **Paul Stoffregen, PJRC and contributors:** Teensy platform, Teensy Audio Library (including AudioStream and stereo Freeverb), Encoder library and Teensy USB support.
- **FortySevenEffects and contributors:** MIDI Library used for the DIN MIDI interface.
- **Frank de Brabander and Marco Schwartz:** LiquidCrystal_I2C library used for the LCD interface.

SPinMicroDexed is a separate MicroDexed-based FM instrument using the shared hardware platform; SPinSynth-T-LCD uses Ricardo's own virtual analog synthesis engine. See the [project history](docs/PROJECT_HISTORY.md) for the timeline and release scope.

## Validated hardware baseline

- Teensy 4.0 at 600 MHz
- PJRC Audio Shield Rev D2
- KEYES DC 3.3 V LCD1602 I2C module at address `0x27`
- Two quadrature rotary encoders with push-buttons
- Isolated 5-pin MIDI-DIN input
- USB configured as **Serial + MIDI + Audio**
- Simultaneous stereo USB Audio and Audio Shield output, including stereo reverb

The 3.3 V LCD connects directly to Teensy pins 18 and 19. No I2C logic-level shifter is used. The LCD has no local pull-ups; the Audio Shield supplies 2.2 kΩ pull-ups to 3.3 V.

![SPinSynth-T-LCD assembled in its housing](docs/images/SPinSynth-T-LCD-Housing.png)

![SPinSynth-T-LCD PCB, LCD and encoder assembly](docs/images/SPinSynth-T-LCD-Hardware.png)

See [docs/hardware/HARDWARE.md](docs/hardware/HARDWARE.md) for pin assignments, controls, electrical details, and the history of the LCD, level-shifter, Audio Shield, and USB investigations.

## Features

- Monophonic, highest-note-priority keyboard behavior
- Sine, saw, square, pulse, and `XTREME` oscillator modes
- Detuned saw, pulse, and sub-oscillator layers in `XTREME` mode
- Oscillator and filter LFOs
- Resonant filter
- Separate amplifier and filter ADSR envelopes
- Portamento, pitch bend, velocity, master tuning, and master volume
- USB MIDI plus 5-pin MIDI-DIN input
- Simultaneous USB Audio and PJRC Audio Shield Rev D2 output
- Stereo Freeverb with MIDI CC36 and HMI dry/wet control
- 16x2 LCD and two-encoder parameter interface
- Heartbeat, CrashReport, uptime, temperature, and startup I2C diagnostics

## Stereo reverb release — V1.1

This release adds PJRC `AudioEffectFreeverbStereo` after the custom synth engine. MIDI CC **36** and the HMI **REVERB MIX** parameter set the dry/wet balance from 0–100%. Startup is fully dry; room size and damping are fixed at `0.5`. Both USB Audio and the Audio Shield receive the same stereo result.

Factory presets and the on-demand parameter dump remain development work and are excluded from this release. See [release notes](docs/releases/RELEASE_1.1.md).

The [hardware circuit diagram](docs/hardware/SPinSynth-T-LCD-Hardware.pdf) documents the shared SPinSynth hardware platform.

## Software dependencies

- Arduino IDE with Teensy support
- PJRC Audio and Encoder libraries
- FortySevenEffects MIDI Library
- Frank de Brabander/Marco Schwartz `LiquidCrystal_I2C` library exposing `LiquidCrystal_I2C(address, columns, rows)`, `init()`, and `backlight()`

The validated workstation used Teensy board package 1.62.0, MIDI Library 5.0.2, LiquidCrystal I2C 1.1.2, and macOS 15.7.3.

## Build and upload

1. Open `SPinSynth-T-LCD/SPinSynth-T-LCD.ino` in Arduino IDE.
2. Select **Teensy 4.0** under **Tools > Board**.
3. Select **Serial + MIDI + Audio** under **Tools > USB Type**.
4. Select 600 MHz CPU speed.
5. Compile and upload.
6. Open Serial Monitor at 9600 baud for startup and operational diagnostics.

The sketch stops compilation when the selected USB type does not include Audio.

After startup, confirm LCD I2C address `0x27` and SGTL5000 address `0x0A` both report status `0`. The SGTL5000 headphone volume is initialized to `0.85`; begin headphone tests with the HMI volume low.

## Tested baseline

The production configuration completed a continuous test exceeding five hours with MIDI-DIN, USB Audio, Audio Shield audio, LCD/HMI, and heartbeat operating normally. CPU temperature reached 54.3 °C. After removal of temporary test code, the production build completed an additional ten-minute CAT with all functions correct at 57.5 °C on a 28 °C day.

See [docs/cat/TESTED_BASELINE.md](docs/cat/TESTED_BASELINE.md) for the complete regression checklist.

See the [documentation index](docs/README.md) for hardware, testing, history and release records.

## Repository layout

```text
SPinSynth-T-LCD/
├── README.md
├── docs/
│   ├── hardware/   # wiring notes and circuit diagram
│   ├── images/     # prototype, PCB and enclosure photos
│   ├── cat/        # tested baseline and regression checklist
│   └── releases/   # release notes
└── SPinSynth-T-LCD/
    ├── SPinSynth-T-LCD.ino
    ├── SPinSynthAudio.*
    ├── SPinSynthHMI.*
    └── synthesis engine sources
```

The nested sketch directory is intentional: Arduino requires the primary `.ino` file to match its containing directory.

## Contributing

This repository is maintained by Ricardo Peculis. External pull requests and contributions are not currently accepted. You are welcome to fork and adapt the software under MIT. See the [maintenance and reuse policy](CONTRIBUTING.md).

## License

The project software and associated documentation are licensed under the [MIT License](LICENSE), copyright © 2015–2026 Ricardo Peculis. Reuse, modification and commercial distribution are permitted subject to preserving the copyright and license notice.

Third-party libraries retain their own licenses and copyright notices; this license does not relicense those dependencies. See [licensing scope and third-party notices](NOTICE.md).
