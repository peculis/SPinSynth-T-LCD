# SPinSynth-T-LCD V1.1 — Stereo Reverb

Firmware baseline: commit `9e117eb22da185ec16197465fa7e578fea0d7a03` (7 August 2026), plus release documentation and hardware assets. No unfinished working-tree firmware changes are included.

## Added behavior

- Global PJRC `AudioEffectFreeverbStereo` effect.
- MIDI CC 36 and HMI REVERB MIX control dry/wet balance from 0–100%.
- Fully dry startup preserves the V1.0 startup sound.
- Room size and damping fixed at 0.5.
- Identical stereo routing to USB Audio and Audio Shield outputs.

Factory presets and the on-demand D/d parameter dump are excluded. Normal uptime and temperature diagnostics remain enabled.

## Validation

Recorded hardware measurements showed maximum total Audio CPU of 7.00%, maximum reverb CPU of 4.49%, and CPU temperature of 57.5–58.1 °C. Freeverb continues processing even with the mix at zero. See [tested baseline](../cat/TESTED_BASELINE.md) for measurements, toolchain and regression details. The five-hour V1.0 endurance result applies to the earlier baseline; it is not a five-hour reverb endurance claim.

## Hardware

See [hardware notes](../hardware/HARDWARE.md) and the [circuit diagram](../hardware/SPinSynth-T-LCD-Hardware.pdf).

## Release preparation build

Compilation check uses Teensy package 1.62.0, Teensy 4.0 at 600 MHz, Faster optimization, Serial + MIDI + Audio, and LiquidCrystal_I2C 1.1.4 available on the current workstation. The earlier hardware validation used LCD library 1.1.2; this new compilation is not a new physical acceptance test.
