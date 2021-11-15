willOS
======

[![Gitter](https://badges.gitter.im/willdurand-kernel/community.svg)](https://gitter.im/willdurand-kernel/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![CircleCI](https://circleci.com/gh/willdurand/willOS/tree/master.svg?style=svg)](https://circleci.com/gh/willdurand/willOS/tree/master)

willOS is a minimal 64 bits kernel (not really an Operating System because it
cannot do a lot of things currently). Some screencasts are available in [this
Twitter thread](https://twitter.com/couac/status/866693418130575361).

## Setting up a development environment

The following dependencies are required to build this project:

* `llvm` (13)
* `nasm`
* `grub-mkrescue` (part of `grub2-common`)
* `xorriso`
* (optional) `qemu`

Currently, this project targets `x86_64` architectures.

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
[`Dockerfile`](https://github.com/willdurand/willOS/blob/master/Dockerfile).
Start by building the toolchain image:

```
$ docker build -t willos/toolchain .
[...]
```

You can then use it with `docker run`:

```
$ docker run -it --rm -v $(pwd):/app willos/toolchain make help
make help
clean                          remove build artifacts
console-font                   compile the (default) kernel console font
debug                          build the OS in debug mode
docs                           build the docs
fmt                            automatically format the code with clang-format
gdb                            build, run the OS in debug mode and enable GDB
kernel                         compile the kernel
libc                           build the libc (userland)
release                        build the OS in release mode
run-debug                      run the OS in debug mode
run-test                       run the OS in test mode
run                            run the OS in release mode
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
$ brew install llvm
```

### Linux

The `tools/install-linux-deps` script is used to install the dependencies. It is
currently used by both the `Dockerfile` and Circle CI.

## Building willOS

The different final files are located in the `build/<arch>/dist/` folder.

### `willosconfig` files

`willosconfig` files are used to configure how a build works. The content must
be compatible with `make`. Here is an example:

```
# LLVM config on MacOS with Homebrew
LLVM_PREFIX = /usr/local/opt/llvm@13/bin/
LLVM_SUFFIX =

# Always enable the Undefined Behavior sanitizer
UBSAN=1

# Sensible logging
ENABLE_CORE_DEBUG=1
ENABLE_PROC_DEBUG=1
ENABLE_SYS_DEBUG=1
ENABLE_USERLAND_DEBUG=1
```

### Debug

To build the ISO in debug mode, run:

```
$ make clean debug
```

To compile the OS in debug mode, build the ISO, and start `qemu` with the OS
loaded, run:

```
$ make clean run-debug
```

#### Logging

In DEBUG mode, logging uses the serial port `COM1` to write various debugging
information. `qemu` is configured to write the output of this serial port to
`./log/debug.log`. `DEBUG` level logs are not necessarily written by default,
though, and it is possible to enable `DEBUG` logs for specific modules like
this:

```
# Enable the debug logs for the "net" and "fs" modules
$ make ENABLE_NET_DEBUG=1 ENABLE_FS_DEBUG=1 clean run-debug
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

There is also `ENABLE_ALL_DEBUG` to turn all debug logs on (which negatively
impacts the performances).

##### Stacktraces

Log files may contain stacktraces without debug symbols if the symbols haven't
been loaded via GRUB:

```
[...]
DEBUG    | src/arch/x86_64/kshell/kshell.c:108:run_command(): command='selftest' argc=1
DEBUG    | src/arch/x86_64/kernel/panic.c:30:kernel_dump_stacktrace(): kernel stacktrace:
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 00000000001163B3 - ???+0x0
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 0000000000115941 - ???+0x0
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 0000000000115BE1 - ???+0x0
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 00000000001152BF - ???+0x0
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace(): 000000000010935B - ???+0x0
```

Use the `tools/fix-stacktrace.py` script to add missing symbol names to the
output:

```
$ ./tools/fix-stacktrace.py build/x86_64/isofiles/boot/symbols.txt log/debug.log
[...]
DEBUG    | src/arch/x86_64/kshell/kshell.c:108:run_command(): command='selftest' argc=1
DEBUG    | src/arch/x86_64/kernel/panic.c:30:kernel_dump_stacktrace(): kernel stacktrace:
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   00000000001163B3 - selftest +0x63
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   0000000000115941 - run_command +0x271
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   0000000000115BE1 - kshell_run +0x181
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   00000000001152BF - kmain +0x107f
DEBUG    | src/arch/x86_64/kernel/panic.c:39:kernel_dump_stacktrace():   000000000010935B - long_mode_start +0x13
```

In addition, you might want to find the corresponding line of code in the source
files by using `llvm-addr2line` or `llvm-symbolizer`:

```
$ llvm-symbolizer-13 --obj=build/x86_64/dist/kernel-x86_64.bin 0x01163B3
print_selftest_header
/Users/william/projects/willos/src/arch/x86_64/kshell/selftest.c:12:3
selftest
/Users/william/projects/willos/src/arch/x86_64/kshell/selftest.c:30:3
```

```
$ llvm-addr2line-13 -e build/x86_64/dist/kernel-x86_64.bin 01163B3
/Users/william/projects/willos/src/arch/x86_64/kshell/selftest.c:12
```

### Release

To compile the OS in release mode, build the ISO, and start `qemu` with the OS
loaded, run:

```
$ make clean run
```

### Boot options

The GRUB configuration offers two choices: the normal mode and the kernel mode.
The normal (or "default") mode will load the
[`/bin/init`](src/userland/init/README.md) program in user mode ("ring 3"). The
kernel mode will load the `kshell` instead and will stay in kernel mode (as its
name implies).

It is possible to change the command line by passing the `GRUB_KERNEL_CMDLINE`
variable to `make` (or add it to your `willosconfig` file):

```
$ make clean GRUB_KERNEL_CMDLINE="/bin/init foo bar" run-debug
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
8. Load a Task State Segment
9. Reload all the data segment registers
10. Call the C `kmain()` function
11. Eventually switch to user mode and call `/bin/init` by default

## Memory management

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

### Memory map

#### Virtual address space

- `0x00000000` to `0x00100000`: the first 1MiB area is reserved
- `0x00100000`: the kernel code and data and multiboot modules
- `0x0020xxxx`: after the end of the previous area (which is computed at runtime
   using the multiboot information), we allocate some space for the frame
   allocator
- `0x10000000`: kernel heap area (we currently have a fixed heap size)
- `0x40000000`: user space

## License

willOS is released under the MIT License. See the bundled
[LICENSE](https://github.com/willdurand/willOS/blob/master/LICENSE.md) file for
details. In addition, some parts of this project have their own licenses
attached (either in the source files or in a `LICENSE` file next to them).
