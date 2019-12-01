willOS
======

[![CircleCI](https://circleci.com/gh/willdurand/willOS/tree/master.svg?style=svg)](https://circleci.com/gh/willdurand/willOS/tree/master)

willOS is a dead-simple 64 bits Operating System.

<p align="center">
<img src="doc/qemu.gif">
<p>

Note: the screencast above is likely outdated.

## Setting up a development environment

The following dependencies are required to build this project:

* `gcc` (`build-essential`)
* `nasm`
* `ld`
* `grub-mkrescue` (part of `grub` or `grub2`)
* `xorriso`
* (optional) `qemu`

This project targets `x86_64` architectures, so you likely need a
cross-compilation toolchain.

### MacOS

Install [Homebrew](https://brew.sh/), then run the following commands:

    $ brew install nasm
    $ brew install xorriso
    $ brew install qemu

You need to tap `hawkw/x86_64-pc-elf` to install the cross-compilation toolchain:

    $ brew tap hawkw/x86_64-pc-elf
    $ brew install x86_64-pc-elf-gcc

You need to tap `hawkw/grub` to install the GRUB utility:

    $ brew tap hawkw/grub
    $ brew install grub --with-x86_64-pc-elf --HEAD

Hat tip to [Eliza Weisman](http://github.com/hawkw) for the taps!

### Linux

See the [circle.yml](circle.yml) file.

## Building willOS

To compile the kernel (ELF 64-bit), run:

    $ make kernel

To build the ISO (this is the default target), run:

    $ make iso

To compile the OS in DEBUG mode, build the ISO, and start `qemu` with the OS
loaded, run:

    $ make debug

**Note:** in DEBUG mode, logging uses the serial port `COM1` to write various
debugging information. `qemu` is configured to write the output of this serial
port to `/tmp/serial.log`.

To compile the OS in normal mode, build the ISO, and start `qemu` with the OS
loaded, run:

    $ make run

## License

willOS is released under the MIT License. See the bundled [LICENSE](LICENSE.md)
file for details.
