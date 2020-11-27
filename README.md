willOS
======

[![Gitter](https://badges.gitter.im/willdurand-kernel/community.svg)](https://gitter.im/willdurand-kernel/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![CircleCI](https://circleci.com/gh/willdurand/willOS/tree/master.svg?style=svg)](https://circleci.com/gh/willdurand/willOS/tree/master)

willOS is a minimal 64 bits kernel (not really an Operating System because it
cannot do a lot of things currently). Some screencasts are available in [this
Twitter thread](https://twitter.com/couac/status/866693418130575361).

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
[`Dockerfile`](https://github.com/willdurand/willOS/blob/master/Dockerfile):

```
$ docker build -t willos/toolchain .
[...]

$ docker run -it --rm -v $(pwd):/app willos/toolchain make help
clean                          remove build artifacts
debug                          build the OS in debug mode
docs                           build the docs
fmt                            automatically format the code with clang-format
gdb                            build, run the OS in debug mode and enable GDB
initrd                         build the init ram disk
iso                            build the image of the OS (.iso)
kernel                         compile the kernel
libc                           build the libc (for userland)
run-debug                      run the OS in debug mode
run-test                       run the OS in test mode
run                            run the OS
test                           run unit tests
userland                       compile the userland programs (statically linked to libc)
version                        print tool versions
```

### MacOS

Install [Homebrew](https://brew.sh/), then run the following commands:

```
$ brew install nasm
$ brew install xorriso
$ brew install qemu
$ brew install x86_64-elf-gcc
```

You need to tap `hawkw/x86_64-pc-elf` and `hawkw/grub` to be able to install the
GRUB utility:

```
$ brew tap hawkw/x86_64-pc-elf
$ brew tap hawkw/grub
$ brew install x86_64-pc-elf-gcc
$ brew install grub --with-x86_64-pc-elf --HEAD
```

Hat tip to [Eliza Weisman](http://github.com/hawkw) for the taps!

### Linux

See the [circle-ci
config](https://github.com/willdurand/willOS/blob/master/.circleci/config.yml).

## Building willOS

To compile the kernel (ELF 64-bit), run:

```
$ make clean kernel
```

To build the ISO (this is the default target), run:

```
$ make clean iso
```

To build the ISO in DEBUG mode, run:

```
$ make clean debug
```

To compile the OS in DEBUG mode, build the ISO, and start `qemu` with the OS
loaded, run:

```
$ make clean run-debug
```

**Note:** in DEBUG mode, logging uses the serial port `COM1` to write various
debugging information. `qemu` is configured to write the output of this serial
port to `./logs/debug.log`.

To compile the OS in normal mode, build the ISO, and start `qemu` with the OS
loaded, run:

```
$ make clean un
```

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

willOS is released under the MIT License. See the bundled
[LICENSE](https://github.com/willdurand/willOS/blob/master/LICENSE.md) file for
details. In addition, some parts of this project have their own licenses
attached (either in the source files or in a `LICENSE` file next to them).
