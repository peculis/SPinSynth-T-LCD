# Contributing

SPinSynth-T-LCD is a hardware-dependent experimental project. Please preserve
the tested USB-Audio behavior while making changes.

## Before opening a pull request

1. Keep unrelated formatting changes out of the patch.
2. Build for Teensy 4.0 with **Serial + MIDI + Audio** selected.
3. Run the checklist in `docs/cat/TESTED_BASELINE.md` on hardware.
4. State the Arduino IDE, Teensy core, and library versions used.
5. Describe the connected hardware and audio/MIDI host.

If a change has only been compile-tested, say so explicitly. Do not report the
Audio Adapter path as tested based on USB-Audio results.

