ENTRY(start)

stack_size = 0x10000; /* 64KiB */

SECTIONS
{
    /* This value should match the load address and entry point of the
     * `mkimage` command and u-boot configuration. */
    . = 0x80008000;

    .text :
    {
        . = ALIGN(4);
        KEEP(*(.text.boot))
         *(.text .text.* .gnu.linkonce.t.*)
         *(.rodata .rodata* .gnu.linkonce.r.*)
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

    .ARM.exidx :
    {
        *(.ARM.exidx*)
    }

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
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
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    }

    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        . = . + stack_size;
        . = ALIGN(8);
        __stack_top = .;
    }

    __heap_start = .;

    /* get rid of unnecessary gcc bits */
    /DISCARD/ :
    {
        *(.ARM.*)
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
