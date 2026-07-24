# Tested baseline

## Confirmed configuration

| Item | Baseline |
| --- | --- |
| Board | Teensy 4.0 |
| USB type | Serial + MIDI + Audio |
| Audio output | USB Audio, mono signal duplicated to stereo |
| MIDI input | USB MIDI |
| Host | Logic Pro on macOS |
| Display | 16x2 I2C LCD at `0x27` |
| Audio Adapter | Disabled / not part of this test |
| Firmware configuration | `SPINSYNTH_USB_ONLY_TEST=1`, `SPINSYNTH_HMI_POLL_TEST=1` |

## Toolchain details to capture

The following details were not preserved with the source. Fill them in after
confirming them on the tested workstation and before publishing a versioned
release:

- Arduino IDE version:
- Teensy board package / Teensyduino version:
- MIDI Library version:
- LiquidCrystal_I2C library name, author, and version:
- macOS version:
- Logic Pro version:

These values matter because multiple incompatible libraries use the
`LiquidCrystal_I2C` name.

## Regression checklist

Use this checklist before merging firmware changes:

- [ ] The sketch compiles for Teensy 4.0 with **Serial + MIDI + Audio**.
- [ ] The Teensy enumerates as both a MIDI and an Audio USB device.
- [ ] USB MIDI note-on and note-off play and stop notes cleanly.
- [ ] Pitch bend and modulation wheel work.
- [ ] USB Audio contains the same signal on the left and right channels.
- [ ] No sustained unexpected noise is present when idle.
- [ ] Master volume and the filter controls respond.
- [ ] Both ADSR envelopes respond.
- [ ] Both encoders and push buttons respond.
- [ ] The LCD initializes and MIDI changes update its displayed values.
- [ ] Serial output reports LCD I2C status `0`.
- [ ] The heartbeat LED continues to blink.
- [ ] The previous Teensy crash report is empty.

## Optional Audio Adapter test

The source includes the I2S and SGTL5000 path, but that path is excluded when
`SPINSYNTH_USB_ONLY_TEST` is `1`. Treat a change to `0` as a separate hardware
test configuration. Verify headphones at low volume first and document the
Audio Adapter revision and wiring before describing that path as supported.

