<!-- doxygen: \page refArch_x86_64 Architecture: x86_64 -->

# x86_64

<!-- doxygen:
## Boards

* \subpage refBoard_pc
* \subpage refBoard_microvm
-->

## Memory management

During the early boot sequence, we identity map the first gigabyte of our kernel
with 512 2MiB pages. When `kmain()` is called, `paging_init()` updates the
mapping:

1. Memory from `0x00000000` to `0x000A0000` is identity mapped (present)
2. Memory from `0x000A0000` to `0x00100000` is identity mapped (present +
   writable)
3. The VBE framebuffer might be identity mapped as well (present + writable,
   and only if `ENABLE_FRAMEBUFFER=1` is passed to `make`; only applicable to
   `pc`)
4. The kernel sections are identity mapped with the correct flags for each
   section
5. The board's boot information is identity mapped (the multiboot information
   on `pc`, the PVH `hvm_start_info` structure on `microvm`)
6. The board's boot modules are identity mapped with the correct flags (in our
   case, it is the init ramdisk)
7. The memory allocated for the frame allocator is then identity mapped

In addition, `paging_init()` will:

1. enable write protection
2. enable the no-execute feature
3. create a guard page

## Memory map

### Virtual address space

- `0x00000000` to `0x00100000`: the first 1MiB area is reserved
- `0x00100000`: the kernel code and data and boot modules
- `0x0020xxxx`: after the end of the previous area (which is computed at
  runtime using the board's boot information), we allocate some space for the
  frame allocator
- `0x10000000`: kernel heap area (we currently have a fixed heap size)
- `0x40000000`: user space
