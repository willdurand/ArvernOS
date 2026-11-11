<!-- doxygen: \page refBoard_microvm microvm -->

# microvm

The x86_64 `microvm` implementation targets QEMU's `microvm` machine type. See:
https://www.qemu.org/docs/master/system/i386/microvm.html

There is no bootloader: QEMU boots the kernel directly (`-kernel`) using the
[PVH ELF note](https://xenbits.xen.org/docs/unstable/misc/pvh.html) (see
`asm/notes.asm`), and hands over boot information (command line, initrd, memory
map) via a `hvm_start_info` structure (see `core/pvh.c`).

Compared to `pc`, this board is headless (serial console only, no VGA/
framebuffer, no keyboard) and has no PCI bus (no networking yet).

## Config file

```
ARCH  = x86_64
BOARD = microvm

# This is only useful when QEMU is used.
CONFIG_SEMIHOSTING = 1
```

## Boot options

It is possible to change the command line by passing the `CMDLINE` variable to
`make` (or add it to your `config` file):

```
$ make clean run-debug CMDLINE="/bin/init foo bar"
```

## Early boot sequence

1. QEMU loads the kernel ELF directly and finds the PVH entry point
2. QEMU builds the `hvm_start_info` structure (command line, initrd, memory
   map) and jumps to the entry point (`start`), passing its address in `ebx`
3. Various checks are performed to make sure we can continue the boot process
4. The page tables and paging are configured and enabled
5. A 64-bit Global Descriptor Table is loaded
6. A Task State Segment is loaded
7. All the data segment registers are reloaded
8. The C `kmain()` function is called, which validates the `hvm_start_info`
   structure and parses the boot information
9. Eventually, the kernel switches to user mode and calls `/bin/init` by
   default

See the [x86_64 architecture page](\ref refArch_x86_64) for memory
management/layout details shared with other boards.
