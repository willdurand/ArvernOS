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

## Memory management

During the early boot sequence, we identity map the first gigabyte of our kernel
with 512 2MiB pages. When `kmain()` is called, `paging_init()` updates the
mapping:

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

1. enable write protection
2. enable the no-execute feature
3. create a guard page

## Memory map

### Virtual address space

- `0x00000000` to `0x00100000`: the first 1MiB area is reserved
- `0x00100000`: the kernel code and data and multiboot modules
- `0x0020xxxx`: after the end of the previous area (which is computed at runtime
  using the multiboot information), we allocate some space for the frame
  allocator
- `0x10000000`: kernel heap area (we currently have a fixed heap size)
- `0x40000000`: user space
