# Repository maintenance and reuse

This repository is maintained by Ricardo Peculis. External pull requests and
contributions are not currently accepted. Ricardo controls changes to this
repository and its releases.

You are welcome to fork, use and adapt the software under the [MIT license](LICENSE).
This maintenance policy does not restrict the rights granted by that license
and does not require you to submit your changes back to this repository.

## Testing your own adaptations

This is a hardware-dependent experimental instrument. Build for Teensy 4.0
with **Serial + MIDI + Audio** and repeat the [regression checklist](docs/cat/TESTED_BASELINE.md)
on your hardware. Record the toolchain, libraries, hardware and host used.
Distinguish compilation checks from physical acceptance tests; USB Audio
validation alone does not establish Audio Shield operation.
