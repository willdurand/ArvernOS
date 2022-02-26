ENTRY(start)

SECTIONS
{
    /* we use command line arguments to configure the actual start address
     * because real hardware expects 0x8000 but QEMU uses 0x10000. */
    . = 0x0;

    .text :
    {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t.*)
    }

    /* this section contains initcall functions as defined in
     * `include/kernel/initcall.h`. */
    .initcalls :
    {
        __initcall_start = .;
        *(SORT_BY_NAME(.initcall_fn_[1-5][1-5]))
        __initcall_end = .;
    }

    .rodata :
    {
        *(.rodata .rodata* .gnu.linkonce.r.*)
    }

    .data :
    {
        *(.data .data*)
    }

    .ARM.exidx :
    {
        *(.ARM.exidx*)
    }

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss .bss*)
        *(COMMON)
        __bss_end = .;
    }

    . = ALIGN(4096);
    __heap_start = .;

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
