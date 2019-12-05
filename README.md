willOS
======

[![CircleCI](https://circleci.com/gh/willdurand/willOS/tree/master.svg?style=svg)](https://circleci.com/gh/willdurand/willOS/tree/master)

willOS is a minimal 64 bits kernel (not really an Operating System because it
cannot do a lot of things currently).

<p align="center">
<img src="docs/qemu.gif">
<p>

Note: the screencast above is likely outdated.

## Setting up a development environment

The following dependencies are required to build this project:

* `gcc` (`build-essential`)
* `nasm`
* `ld`
* `grub-mkrescue` (part of `grub2-common`)
* `xorriso`
* (optional) `qemu`

This project targets `x86_64` architectures, so you likely need a
cross-compilation toolchain.

### Docker (recommended way)

Use [Docker](https://docs.docker.com/) with the provided
[`Dockerfile`](./Dockerfile):

   $ docker build -t willos/toolchain .
   $ docker run --rm -v $(pwd):/app willos/toolchain make

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

See the [circle-ci config](.circleci/config.yml).

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

## Early boot sequence

1. The BIOS loads the bootloader (GRUB) from the ISO
2. The bootloader reads the kernel executable and finds the multiboot header
3. The bootloader copies the `.boot` and `.text` sections to memory (to
   addresses `0x100000` and `0x100020`)
4. The bootloader jumps to the entry point (at `0x100020`)
5. Various checks are performed to make sure we can continue the boot process
6. Page table and paging are configured and enabled
7. Load a 64-bit Global Descriptor Table
8. Reload all the data segment registers
9. Call the C `kmain()` function

## License

willOS is released under the MIT License. See the bundled [LICENSE](LICENSE.md)
file for details.
