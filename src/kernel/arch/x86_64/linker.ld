/* cf. https://intermezzos.github.io/book/hello-world.html */

ENTRY(start)

SECTIONS {
    /* sets the load address of the first section to 1 MiB (convention) */
    . = 1M;

    .rodata :
    {
        /* ensure that the multiboot header is at the beginning */
        KEEP(*(.multiboot_header))
        *(.rodata .rodata.*)
        . = ALIGN(4K);
    }

    /* next we put the .text section */
    .text :
    {
        *(.text. .text.*)
        . = ALIGN(4K);
    }

    /* this section contains initcall functions as defined in
     * `include/kernel/init.h`. */
    .initcalls :
    {
        __initcall_start = .;
        *(.initcall_functions)
        __initcall_end = .;
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
