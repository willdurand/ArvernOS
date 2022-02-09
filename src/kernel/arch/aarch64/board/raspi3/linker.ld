ENTRY(start)

SECTIONS
{
    . = 0x80000;

    .text :
    {
        . = ALIGN(4);
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t.*)
    }

    /* this section contains initcall functions as defined in
     * `include/kernel/initcall.h`. */
    .initcalls :
    {
        . = ALIGN(4);
        __initcall_start = .;
        *(SORT_BY_NAME(.initcall_fn_[1-5][1-5]))
        . = ALIGN(4);
        __initcall_end = .;
    }

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata .rodata* .gnu.linkonce.r.*)
    }

    .data :
    {
        . = ALIGN(4);
        *(.data*)
    }

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start = .;
        *(.bss .bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}

__bss_size = (__bss_end - __bss_start) >> 3;
