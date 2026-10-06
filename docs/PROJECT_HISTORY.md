# SPinSynth-T-LCD project history

## Original SPinSynth — 2015

Ricardo Peculis began the project in February 2015 and implemented the monophonic virtual analog synthesis engine: oscillator, filter, amplifier, envelopes, LFOs and MIDI control. SPinSynth-T targeted Teensy 3.1 and its DAC output.

## Teensy 4.0 and LCD port — June 2026

The port introduced the SPinSynthAudio AudioStream adapter, 16-bit audio integration and SPinSynthHMI LCD/encoder component while preserving the custom synthesis engine and MIDI CC controls. USB Audio provided the initial working audio baseline.

## Validated complete hardware baseline — August 2026

The 6 August firmware commit enabled the production Audio Shield path alongside USB Audio and removed temporary diagnostic test modes. Native 3.3 V LCD operation eliminated the level shifter. Replacing the USB cable and cleaning contacts resolved the observed disconnections. The recorded V1.0 endurance test exceeded five hours.

V1.0 was published on 6 August 2026. See [hardware history](hardware/HARDWARE.md) and [CAT records](cat/TESTED_BASELINE.md) for evidence and test conditions.

## Stereo reverb — V1.1

The 7 August development commit `9e117eb` added PJRC stereo Freeverb with MIDI CC36 and HMI dry/wet control. Measurements recorded maximum total Audio CPU of 7.00% and reverb CPU of 4.49%. This is a measured resource cost, not a synthesis performance improvement claim.

V1.1 was published on 5 October 2026 with the committed stereo reverb firmware, hardware circuit diagram and new PCB/enclosure photos. See [release notes](releases/RELEASE_1.1.md).

Factory presets and the on-demand parameter dump remain unfinished development work and are excluded from V1.1. The five-hour V1.0 endurance result does not constitute a five-hour V1.1 reverb test.

## Collaboration

Ricardo is the original synth author and directs the hardware, musical objectives and physical acceptance tests. OpenAI Codex assisted with code analysis, implementation, diagnosis, compilation and repository documentation. PJRC and the MIDI/LCD library authors supplied the platform and supporting libraries acknowledged in the [README](../README.md).
