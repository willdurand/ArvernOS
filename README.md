# ArvernOS

[![Gitter](https://badges.gitter.im/willdurand-kernel/community.svg)](https://gitter.im/willdurand-kernel/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![CircleCI](https://circleci.com/gh/willdurand/ArvernOS/tree/master.svg?style=svg)](https://circleci.com/gh/willdurand/ArvernOS/tree/master)

ArvernOS (formerly known as "willOS") is a minimal 64 bits kernel (not really an
Operating System because it cannot do a lot of things currently). Some
screencasts are available in [this Twitter thread][twitter-thread].

## Goals

The main goal of this project is to learn about operating systems, kernel
development, different architectures and improve my C skills. ArvernOS is a
monolithic (and hopefully modular) [kernel](./src/kernel/README.md) with a
unified homemade [libc/libk](./src/libc/README.md), and
[userland](./src/userland/README.md) programs.

The roadmap isn't clearly defined yet and it mainly depends on what I'd like to
work on when I have time to spend on this project.

## Setting up a development environment

The following dependencies are required to build this project:

- `llvm` (version 13 currently)
- `make`
- (optional) `qemu`

Currently, this project targets `x86_64` architectures and requires the
following extra dependencies:

- `nasm`
- `grub-mkrescue`
- `xorriso`

The recommended way to develop on this project is to use Docker (see below).

### Getting the sources

This project contains [git
submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). You have to
clone the main project as well as the submodules, either by using this command:

```
$ git clone --recurse-submodules <url pointing to this repo>
```

or by using this command if you already have a copy of this git repository:

```
$ git submodule update --init
```

### Docker (recommended way)

Use [Docker](https://docs.docker.com/) with the provided
[`Dockerfile`](https://github.com/willdurand/ArvernOS/blob/master/Dockerfile).
You can either use the [`willdurand/arvernos-toolchain`
image from DockerHub][dockerhub-toolchain] or build your own:

```
$ docker build -t willdurand/arvernos-toolchain .
[...]
```

You can then use it with `docker run`:

```
$ docker run -it --rm -v $(pwd):/app willdurand/arvernos-toolchain make help
ArvernOS - available commands for arch=x86_64

clean                          remove build artifacts
debug                          build the OS in debug mode
docs                           build the docs
fmt                            automatically format the code with clang-format
gdb                            build, run the OS in debug mode and enable GDB
libc                           build the libc (userland)
release                        build the OS in release mode
run-debug                      run the OS in debug mode
run-release                    run the OS in release mode
run-test                       run the OS in test mode
test                           run unit tests
userland                       compile the userland programs (statically linked to libc)
version                        print tool versions
```

### MacOS

Install [Homebrew](https://brew.sh/), then run the following commands:

```
$ brew install nasm xorriso qemu llvm
```

### Linux

The `tools/install-linux-deps` script is used to install the dependencies. It is
currently used by both the `Dockerfile` and Circle CI.

## Building ArvernOS

You first need to install the development dependencies in order to build ArvernOS.
The different final files are located in the `build/<arch>/dist/` folder.

### Debug mode

To build the image in debug mode, run:

```
$ make clean debug
```

To compile the OS in debug mode, build the image, and start `qemu` with the OS
loaded, run:

```
$ make clean run-debug
```

### Release mode

To compile the OS in release mode, build the image, and start `qemu` with the OS
loaded, run:

```
$ make clean run-release
```

## Configuration

### config files

`config` files are used to configure how a build works. The content must
be compatible with `make`. Here is an example:

```
# LLVM config on MacOS with Homebrew
LLVM_PREFIX = /usr/local/opt/llvm@13/bin/
LLVM_SUFFIX =

# Always enable the Undefined Behavior sanitizer
UBSAN = 1
```

### Logging

The logging layer outputs kernel logs using the [ndjson][ndjson] format. Tools
such as [pino-pretty][] can parse those logs in order to format them in a more
human friendly manner.  Logs are sent to a serial port.

QEMU is configured to redirect these logs to file in the `log/` folder. One can
use the following command in a different terminal to follow the logs when the
kernel is running in QEMU:

```
$ tail -f log/x86_64-debug.log | pino-pretty
```

There is a global configuration option for the verbosity of the kernel:
`LOGGING_LEVEL`. In addition, the different components of the kernel can be
configured separately as well:

- `LOGGING_LEVEL_CONFIG`
- `LOGGING_LEVEL_CORE`
- `LOGGING_LEVEL_FS`
- `LOGGING_LEVEL_MMU`
- `LOGGING_LEVEL_NET`
- `LOGGING_LEVEL_PROC`
- `LOGGING_LEVEL_SYS`
- `LOGGING_LEVEL_USERLAND`

The possible _level_ values are:

|            |       |       |      |      |       |       |          |
|:-----------|-------|-------|------|------|-------|-------|---------:|
| **Level:** | trace | debug | info | warn | error | fatal | silent   |
| **Value:** | 10    | 20    | 30   | 40   | 50    | 60    | Infinity |


#### Stack traces

The kernel might dump a stack trace when a panic error has occurred. There is a
script to fix the stack trace is the symbols are not attached:
`tools/fix-stacktrace.py`.

In addition, you might want to find the corresponding line of code in the source
files by using `llvm-addr2line` or `llvm-symbolizer`:

```
$ llvm-symbolizer-13 --obj=build/x86_64/dist/kernel-x86_64.bin 0x01163B3
print_selftest_header
/path/to/ArvernOS/src/kernel/arch/x86_64/kshell/selftest.c:12:3
selftest
/path/to/ArvernOS/src/kernel/arch/x86_64/kshell/selftest.c:30:3
```

```
$ llvm-addr2line-13 -e build/x86_64/dist/kernel-x86_64.bin 01163B3
/path/to/ArvernOS/src/kernel/arch/x86_64/kshell/selftest.c:12
```

### x86_64 options

#### Boot options

The GRUB configuration offers two choices: the normal mode and the kernel mode.
The normal (or "default") mode will load the
[`/bin/init`](src/userland/init/README.md) program in user mode ("ring 3"). The
kernel mode will load the `kshell` instead and will stay in kernel mode (as its
name implies).

It is possible to change the command line by passing the `CMDLINE` variable to
`make` (or add it to your `config` file):

```
$ make clean run-debug CMDLINE="/bin/init foo bar"
```

Note: it is also possible to set `KERNEL_CMDLINE`, which is passed to the kernel
in... kernel mode.

## Some information about the x86_64 implementation

### Early boot sequence

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

### Memory management

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
[LICENSE](https://github.com/willdurand/ArvernOS/blob/master/LICENSE.md) file for
details. In addition, some parts of this project have their own licenses
attached (either in the source files or in a `LICENSE` file next to them).

[twitter-thread]: https://twitter.com/couac/status/866693418130575361
[dockerhub-toolchain]: https://hub.docker.com/repository/docker/willdurand/arvernos-toolchain
[pino-pretty]: https://github.com/pinojs/pino-pretty
[ndjson]: http://ndjson.org/
