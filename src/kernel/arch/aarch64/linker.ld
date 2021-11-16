ENTRY(start)

SECTIONS
{
    . = 0x80000;

    .text :
    {
        KEEP(*(.text.boot))
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
        . = ALIGN(4K);
    }

    .data :
    {
        *(.data*)
        . = ALIGN(4K);
    }

    __bss_start = .;
    .bss :
    {
        *(.bss*)
        . = ALIGN(4K);
    }
    __bss_end = .;

    /DISCARD/ :
    {
        *(.comment)
    }
}

__bss_size = (__bss_end - __bss_start) >> 3;
