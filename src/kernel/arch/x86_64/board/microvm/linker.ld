/* cf. https://intermezzos.github.io/book/first-edition/hello-world.html */

ENTRY(start)

SECTIONS {
    /* sets the load address of the first section to 1 MiB (convention) */
    . = 1M;

    __kernel_start = .;

    .notes :
    {
        __notes_start = .;
        *(.notes)
        __notes_end = .;
    }

    .text ALIGN(4K) :
    {
        __text_start = .;
        *(.text .text.*)
        __text_end = .;
    }

    /* this section contains initcall functions as defined in
     * `include/kernel/initcall.h`. */
    .initcalls ALIGN(4K) :
    {
        __initcall_start = .;
        *(SORT_BY_NAME(.initcall_fn_[1-5][1-5]))
        __initcall_end = .;
    }

    .rodata ALIGN(4K) :
    {
        __rodata_start = .;
        *(.rodata .rodata.*)
        __rodata_end = .;
    }

    /* read-write data (initialized) */
    .data ALIGN(4K) :
    {
        __data_start = .;
        *(.data .data.*)
        __data_end = .;
    }

    /* read-write data (uninitialized) and stack */
    .bss ALIGN(4K) :
    {
        __bss_start = .;
        *(.bss .bss.*)
        __bss_end = .;
    }

    __kernel_end = .;

    /* get rid of unnecessary bits */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
