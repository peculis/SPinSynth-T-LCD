# Hardware and controls

## Pin assignment

The current Teensy 4.0 assignment is:

| Function | Teensy pin |
| --- | ---: |
| Left encoder A | 2 |
| Left encoder B | 3 |
| Left encoder push button | 4 |
| Right encoder A | 5 |
| Right encoder B | 9 |
| Right encoder push button | 14 |
| LCD SDA | 18 |
| LCD SCL | 19 |

Encoder push buttons are configured with the Teensy's internal pull-up
resistors, so each button connects its pin to ground when pressed.

The LCD is configured as 16 columns by 2 rows at I2C address `0x27`. Pins 18 and
19 are the Teensy 4.0 default I2C pins; they are selected by the `Wire` library,
not declared directly in the sketch.

The right encoder uses pins 5 and 9 to stay clear of the Audio Adapter's SPI
connections.

## HMI behavior

- Turn the left encoder to select a parameter.
- Turn the right encoder to change the selected value.
- Press the left button to move to the next parameter group.
- Press the right button to toggle fine/coarse editing.
- Press both buttons to return to the home screen.
- Long-press the right button to restore the selected parameter's default.
- On the main pulse-waveform screen, press the left button to enter or leave
  pulse-width editing.

MIDI changes are reflected on the LCD.

## MIDI interfaces

The sketch listens to:

- USB MIDI, when **Serial + MIDI + Audio** is selected
- DIN MIDI on Teensy `Serial1`, through the FortySevenEffects MIDI Library

DIN MIDI input requires the usual isolated MIDI input circuit; do not connect a
5-pin DIN socket directly to a Teensy GPIO pin.

## Main MIDI CC map

| CC | Function |
| ---: | --- |
| 1 | Mod wheel |
| 7 | Master volume |
| 16–17 | Oscillator LFO rate and amount |
| 18 | Mod-wheel function |
| 19 | Filter envelope amount |
| 35 | Portamento |
| 37–44 | Oscillator shape controls |
| 71, 74 | Filter resonance and cutoff |
| 72–73, 75, 79 | Filter envelope |
| 76–77, 93 | Filter LFO |
| 80–83 | Amplifier envelope |
| 91 | Oscillator LFO waveform |

Pitch bend is handled separately. The complete mapping and value ranges are
documented near the top of the primary sketch.
