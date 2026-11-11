/* cf. https://intermezzos.github.io/book/first-edition/hello-world.html */

ENTRY(start)

SECTIONS {
    /* sets the load address of the first section to 1 MiB (convention) */
    . = 1M;

    __kernel_start = .;

    .text ALIGN(4K) :
    {
        /* ensure that the multiboot header is at the beginning */
        KEEP(*(.multiboot_header))
        *(.text .text.*)
    }

    /* this section contains initcall functions as defined in
     * `include/kernel/initcall.h`. */
    .initcalls ALIGN(4K) :
    {
        __initcall_start = .;
        *(SORT_BY_NAME(.initcall_fn_[1-5][1-5]))
        __initcall_end = .;
    }

    /* this section contains PCI driver registrations as defined in
     * `src/kernel/arch/x86_64/board/pc/core/pci.h`. */
    .pci_drivers ALIGN(4K) :
    {
        __pci_drivers_start = .;
        *(.pci_drivers)
        __pci_drivers_end = .;
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata .rodata.*)
    }

    /* read-write data (initialized) */
    .data ALIGN(4K) :
    {
        *(.data .data.*)
    }

    /* read-write data (uninitialized) and stack */
    .bss ALIGN(4K) :
    {
        *(.bss .bss.*)
    }

    __kernel_end = .;

    /* get rid of unnecessary bits */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
