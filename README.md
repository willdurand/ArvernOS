<!-- doxygen: \mainpage Introduction -->

# ArvernOS

[![Gitter](https://badges.gitter.im/willdurand-kernel/community.svg)](https://gitter.im/willdurand-kernel/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![CircleCI](https://circleci.com/gh/willdurand/ArvernOS/tree/master.svg?style=svg)](https://circleci.com/gh/willdurand/ArvernOS/tree/master)

ArvernOS (formerly known as "willOS") is a minimal and experimental monolithic
kernel (not really an Operating System because it cannot do a lot of things
currently). Some screencasts are available in [this Twitter
thread][twitter-thread].

## Goals

The main goal of this project is to learn about operating systems, kernel
development, different architectures and improve my C skills. ArvernOS is a
monolithic (and hopefully modular) [kernel](./src/kernel/README.md) with a
unified homemade [libc/libk](./src/libc/README.md), and
[userland](./src/userland/README.md) programs.

The roadmap isn't clearly defined yet and it mainly depends on what I'd like to
work on when I have time to spend on this project.

## Architectures, boards and tiers

ArvernOS supports the architectures and boards listed below. Support for
different architectures and boards are organized into two tiers, each with a
different set of guarantees.

### Tier 1

Tier 1 is the reference implementation and very likely the most advanced.

Current Tier 1 archs/boards:

- **x86_64**
  - generic

### Tier 2

Tier 2 is guaranteed to build but not all features have been implemented yet.
Features are defined by the reference implementation and once feature parity is
achieved, an architecture and/or board should move to Tier 1. Boards for which
we cannot run enough tests in CI (e.g., because QEMU does not support the board)
should stay in Tier 2, though.

Current Tier 2 archs/boards:

- **aarch32**
  - [Lichee Pi Nano](./src/kernel/arch/aarch32/board/licheepi-nano/README.md)
- **aarch64**
  - [Raspberry Pi 3](./src/kernel/arch/aarch64/board/raspi3/README.md)

## Hacking on ArvernOS

This section (and its sub-sections) are written for everyone interested in
building and working on ArvernOS.

### Setting up a development environment

The following dependencies are required to build this project:

- `llvm` (version 13 currently)
- `make`
- `qemu`

If you want to work on the `x86_64` architecture, you'll need the following
extra dependencies:

- `nasm`
- `grub-mkrescue`
- `xorriso`

If you want to work on ARM architectures (`aarch32` or `aarch64`), you'll need
the following extra dependencies:

- `gcc-arm-none-eabi`
- `u-boot-tools`

Note: The recommended way to work on this project is to use Docker.

#### Getting the sources

This project contains [git submodules][git-submodules]. You have to clone the
main project as well as the submodules, either by using this command:

```
$ git clone --recurse-submodules <url pointing to this repo>
```

or by using this command if you already have a copy of this git repository:

```
$ git submodule update --init
```

#### Docker (recommended way)

Use [Docker](https://docs.docker.com/) with the provided
[`Dockerfile`][dockerfile]. You can either use the
[`willdurand/arvernos-toolchain` image from DockerHub][dockerhub-toolchain] or
build your own:

```
$ docker build -t willdurand/arvernos-toolchain .
[...]
```

You can then use it with `docker run`:

```
$ docker run -it --rm -v $(pwd):/app willdurand/arvernos-toolchain make help
ArvernOS - available commands for arch=aarch64 board=raspi3

clean                          remove build artifacts
debug                          build the project in debug mode
docs                           build the docs
fmt                            automatically format the code with clang-format
gdb                            build, run the project in debug mode and enable GDB
help                           show this help message
libc                           build the libc
release                        build the project in release mode
run-debug                      run the project in debug mode
run-release                    run the project in release mode
run-test                       run the project in test mode
test                           run the unit tests
userland                       compile the userland programs (statically linked to libc)
version                        print tool versions
```

#### MacOS

Install [Homebrew](https://brew.sh/), then run the following commands:

```
$ brew install nasm xorriso qemu llvm u-boot-tools
```

#### Linux

The `tools/install-linux-deps` script is used to install the dependencies. It is
currently used by both the `Dockerfile` and Circle CI.

### Building ArvernOS

You first need to install the development dependencies in order to build
ArvernOS. The different final files are located in the `build/<arch>/dist/` or
`build/<arch>/<board>/dist/` folder.

#### Debug mode

To build the image in debug mode, run:

```
$ make clean debug
```

To compile the OS in debug mode, build the image, and start `qemu` with the OS
loaded, run:

```
$ make clean run-debug
```

Note: Some boards aren't supported in QEMU.

##### Logging

In DEBUG mode, logging very likely uses the serial port `COM1` to write various
debugging information. `qemu` is configured to write the output of this serial
port to a logfile in `./log/`. `DEBUG` level logs are not necessarily written by
default, though, and it is possible to enable `DEBUG` logs for specific modules
like this:

```
# Enable the debug logs for the "net" and "fs" modules
$ make clean run-debug ENABLE_NET_DEBUG=1 ENABLE_FS_DEBUG=1
```

The available debug variables are:

- `ENABLE_CONFIG_DEBUG`
- `ENABLE_CORE_DEBUG`
- `ENABLE_FS_DEBUG`
- `ENABLE_MMU_DEBUG`
- `ENABLE_NET_DEBUG`
- `ENABLE_PROC_DEBUG`
- `ENABLE_SYS_DEBUG`
- `ENABLE_USERLAND_DEBUG`

Note: There is also `ENABLE_ALL_DEBUG` to turn all debug logs on (which
negatively impacts the performances).

##### Stacktraces

Log files may contain stacktraces without debug symbols if the symbols haven't
been loaded:

```
[...]
DEBUG    | src/kernel/arch/x86_64/kshell/kshell.c:108:run_command(): command='selftest' argc=1
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:30:kernel_dump_stacktrace(): kernel stacktrace:
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 00000000001163B3 - ???+0x0
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 0000000000115941 - ???+0x0
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 0000000000115BE1 - ???+0x0
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 00000000001152BF - ???+0x0
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 000000000010935B - ???+0x0
```

Use the `tools/fix-stacktrace.py` script to add missing symbol names to the
output:

```
$ ./tools/fix-stacktrace.py build/x86_64/dist/symbols.txt log/x86_64-debug.log
[...]
DEBUG    | src/kernel/arch/x86_64/kshell/kshell.c:108:run_command(): command='selftest' argc=1
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:30:kernel_dump_stacktrace(): kernel stacktrace:
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   00000000001163B3 - selftest +0x63
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   0000000000115941 - run_command +0x271
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   0000000000115BE1 - kshell_run +0x181
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   00000000001152BF - kmain +0x107f
DEBUG    | src/kernel/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   000000000010935B - long_mode_start +0x13
```

In addition, you might want to find the corresponding line of code in the source
files by using `llvm-addr2line` or `llvm-symbolizer`:

```
$ llvm-symbolizer-13 --obj=build/x86_64/dist/kernel-x86_64.bin 0x01163B3
print_selftest_header
/path/to/ArvernOS/src/kernel/kshell/selftest.c:12:3
selftest
/path/to/ArvernOS/src/kernel/kshell/selftest.c:30:3
```

```
$ llvm-addr2line-13 -e build/x86_64/dist/kernel-x86_64.bin 01163B3
/path/to/ArvernOS/src/kernel/kshell/selftest.c:12
```

#### Release mode

To compile the OS in release mode, build the image, and start `qemu` with the OS
loaded, run:

```
$ make clean run-release
```

#### config files

`config` files are used to configure how a build works. The content must be
compatible with `make`. Here is an example:

```
# LLVM config on MacOS with Homebrew
LLVM_PREFIX = /usr/local/opt/llvm@13/bin/
LLVM_SUFFIX =

# Always enable the Undefined Behavior sanitizer
UBSAN = 1

# Sensible logging
ENABLE_CORE_DEBUG     = 1
ENABLE_PROC_DEBUG     = 1
ENABLE_SYS_DEBUG      = 1
ENABLE_USERLAND_DEBUG = 1
```

### About x86_64

The `x86_64` implementation uses GRUB as bootloader.

#### Boot options

The GRUB configuration offers two choices: the normal mode and the kernel mode.
The normal (or "default") mode will load the
[`/bin/init`](./src/userland/init/README.md) program in user mode ("ring 3").
The kernel mode will load the `kshell` instead and will stay in kernel mode (as
its name implies).

It is possible to change the command line by passing the `CMDLINE` variable to
`make` (or add it to your `config` file):

```
$ make clean run-debug CMDLINE="/bin/init foo bar"
```

Note: It is also possible to set `KERNEL_CMDLINE`, which is passed to the kernel
in... kernel mode.

#### Early boot sequence

1. The BIOS loads the bootloader (GRUB) from the ISO
2. The bootloader reads the kernel executable and finds the multiboot header
3. The bootloader copies the `.boot` and `.text` sections to memory (to
   addresses `0x100000` and `0x100020`)
4. The bootloader jumps to the entry point (at `0x100020`)
5. Various checks are performed to make sure we can continue the boot process
6. Page table and paging are configured and enabled
7. Load a 64-bit Global Descriptor Table
8. Load a Task State Segment
9. Reload all the data segment registers
10. Call the C `kmain()` function
11. Eventually switch to user mode and call `/bin/init` by default

#### Memory management

During the early boot sequence, we identity map the first gigabyte of our kernel
with 512 2MiB pages. When `kmain()` is called, we call `paging_init()` to update
the mapping:

1. Memory from `0x00000000` to `0x000A0000` is identity mapped (present)
2. Memory from `0x000A0000` to `0x00100000` is identity mapped (present +
   writable)
3. The VBE framebuffer might be identity mapped as well (present + writable,
   and only if `ENABLE_FRAMEBUFFER=1` is passed to `make`)
4. The kernel sections are identity mapped with the correct flags for each
   section
5. The multiboot information is identity mapped
6. The multiboot modules are identity mapped with the correct flags (in our
   case, it is the init ramdisk)
7. The memory allocated for the frame allocator is then identity mapped

In addition, `paging_init()` will:

1. enable the write protection
2. enable the no-execute feature
3. create a guard page

#### Memory map

##### Virtual address space

- `0x00000000` to `0x00100000`: the first 1MiB area is reserved
- `0x00100000`: the kernel code and data and multiboot modules
- `0x0020xxxx`: after the end of the previous area (which is computed at runtime
  using the multiboot information), we allocate some space for the frame
  allocator
- `0x10000000`: kernel heap area (we currently have a fixed heap size)
- `0x40000000`: user space

## License

ArvernOS is released under the MIT License. See the bundled
[LICENSE][license] file for
details. In addition, some parts of this project have their own licenses
attached (either in the source files or in a `LICENSE` file next to them).

[git-submodules]: https://git-scm.com/book/en/v2/Git-Tools-Submodules
[twitter-thread]: https://twitter.com/couac/status/866693418130575361
[dockerhub-toolchain]: https://hub.docker.com/repository/docker/willdurand/arvernos-toolchain
[dockerfile]: https://github.com/willdurand/ArvernOS/blob/master/Dockerfile
[license]: https://github.com/willdurand/ArvernOS/blob/master/LICENSE.md
