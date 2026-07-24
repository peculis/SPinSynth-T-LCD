# SPinSynth-T-LCD

SPinSynth-T-LCD is a monophonic software synthesizer for the Teensy 4.0. It
combines a custom synthesis engine with the PJRC Teensy Audio Library, USB MIDI,
USB Audio, a 16x2 I2C LCD, and two rotary encoders.

The current baseline was tested without an Audio Adapter: MIDI is received over
USB and the mono synth output is sent to both channels of the Teensy USB Audio
device. Logic Pro on macOS was used as the host.

> **Project status:** experimental, working hardware baseline. This is not a
> finished product. The USB-Audio path is tested; the optional SGTL5000 Audio
> Adapter path is implemented but is not part of this baseline.

## Features

- Monophonic, highest-note-priority keyboard behavior
- Sine, saw, square, pulse, and `XTREME` oscillator modes
- Detuned saw, pulse, and sub-oscillator layers in `XTREME` mode
- Oscillator and filter LFOs
- Resonant filter
- Separate amplifier and filter ADSR envelopes
- Portamento, pitch bend, velocity, master tuning, and master volume
- USB MIDI plus 5-pin DIN MIDI input
- Stereo USB Audio output carrying the same mono signal on both channels
- 16x2 LCD and two-encoder parameter interface

## Hardware

- Teensy 4.0
- 16x2 HD44780-compatible LCD with an I2C backpack at address `0x27`
- Two quadrature rotary encoders with push buttons
- Optional: 5-pin DIN MIDI input hardware
- Optional and currently untested: PJRC Teensy Audio Adapter

See [docs/HARDWARE.md](docs/HARDWARE.md) for the pin assignment and controls.

## Software dependencies

Install Teensy support in the Arduino IDE, including the libraries bundled with
Teensyduino. The sketch also uses:

- PJRC Audio
- PJRC Encoder
- FortySevenEffects MIDI Library
- A `LiquidCrystal_I2C` library exposing
  `LiquidCrystal_I2C(address, columns, rows)`, `init()`, and `backlight()`

The precise IDE, Teensy core, and third-party library versions used for the
hardware test were not captured. Record them in
[docs/TESTED_BASELINE.md](docs/TESTED_BASELINE.md) before tagging a release.

## Build and upload

1. Open `SPinSynth-T-LCD/SPinSynth-T-LCD.ino` in the Arduino IDE.
2. Select **Teensy 4.0** under **Tools > Board**.
3. Select **Serial + MIDI + Audio** under **Tools > USB Type**.
4. Compile and upload.
5. Open the Serial Monitor at 9600 baud if startup diagnostics are needed.

The sketch intentionally stops compilation when the selected USB type does not
include Audio.

## Tested USB-Audio setup

The checked-in configuration uses:

```cpp
#define SPINSYNTH_USB_ONLY_TEST 1
#define SPINSYNTH_HMI_POLL_TEST 1
```

`SPINSYNTH_USB_ONLY_TEST` keeps the SGTL5000 Audio Adapter disabled and routes
audio through USB. Do not change it when reproducing the baseline.

After upload:

1. Connect the Teensy directly to the Mac by USB.
2. Select the Teensy as a stereo audio input in the host or DAW.
3. Select the Teensy MIDI port as the MIDI destination.
4. Play a note and confirm that the same mono signal appears on both USB input
   channels.
5. Confirm that the LCD responds to both encoders and that the Serial Monitor
   reports the LCD at I2C address `0x27`.

More detail and the release checklist are in
[docs/TESTED_BASELINE.md](docs/TESTED_BASELINE.md).

## Repository layout

```text
SPinSynth-T-LCD/
├── README.md
├── docs/
└── SPinSynth-T-LCD/
    ├── SPinSynth-T-LCD.ino
    ├── SPinSynthAudio.*
    ├── SPinSynthHMI.*
    └── synthesis engine sources
```

The nested sketch directory is intentional: Arduino requires the primary
`.ino` file to match its containing directory.

## Contributing

Please keep changes small and test the USB-Audio baseline after modifications.
See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

No open-source license has been selected yet. Until the copyright owner adds a
license, the source is provided for viewing only under standard copyright law.

