/* cf. https://intermezzos.github.io/book/hello-world.html */

ENTRY(start)

SECTIONS {
    /* sets the load address of the first section to 1 MiB (convention) */
    . = 1M;

    .text :
    {
        /* ensure that the multiboot header is at the beginning */
        KEEP(*(.multiboot_header))
        *(.text .text.*)
        . = ALIGN(4K);
    }

    /* this section contains initcall functions as defined in
     * `include/kernel/initcall.h`. */
    .initcalls :
    {
        __initcall_start = .;
        *(SORT_BY_NAME(.initcall_fn_[1-5][1-5]))
        __initcall_end = .;
        . = ALIGN(4K);
    }

    .rodata :
    {
        *(.rodata .rodata.*)
        . = ALIGN(4K);
    }

    /* read-write data (initialized) */
    .data :
    {
        *(.data .data.*)
        . = ALIGN(4K);
    }

    /* read-write data (uninitialized) and stack */
    .bss :
    {
        *(.bss .bss.*)
        . = ALIGN(4K);
    }

    /* get rid of unnecessary gcc bits */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
