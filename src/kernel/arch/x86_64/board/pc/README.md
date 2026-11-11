<!-- doxygen: \page refBoard_pc pc -->

# pc

The x86_64 `pc` implementation uses GRUB as the bootloader.

## Config file

```
ARCH  = x86_64
BOARD = pc

# This is only useful when QEMU is used.
CONFIG_SEMIHOSTING = 1
```

## Boot options

The GRUB configuration offers two choices: the normal mode and the kernel mode.
The normal (or "default") mode will load the `/bin/init` program in user mode
("ring 3"). The kernel mode will load the `kshell` instead and will stay in
kernel mode (as its name implies).

It is possible to change the command line by passing the `CMDLINE` variable to
`make` (or add it to your `config` file):

```
$ make clean run-debug CMDLINE="/bin/init foo bar"
```

Note: It is also possible to set `KERNEL_CMDLINE`, which is passed to the kernel
when it boots in kernel mode.

## Early boot sequence

1. The BIOS loads the bootloader (GRUB) from the ISO
2. The bootloader reads the kernel executable and finds the multiboot header
   (placed at the beginning of the `.text` section)
3. The bootloader loads the kernel sections into memory (the `.text` section is
   loaded at `0x100000`)
4. The bootloader jumps to the entry point (`start`)
5. Various checks are performed to make sure we can continue the boot process
6. The page tables and paging are configured and enabled
7. A 64-bit Global Descriptor Table is loaded
8. A Task State Segment is loaded
9. All the data segment registers are reloaded
10. The C `kmain()` function is called
11. Eventually, the kernel switches to user mode and calls `/bin/init` by
    default

See the [x86_64 architecture page](\ref refArch_x86_64) for memory
management/layout details shared with other boards.
