; cf. https://intermezzos.github.io/book/multiboot-headers.html
section .multiboot_header

%define MULTIBOOT2_MAGIC_NUMBER	0xe85250d6
%define PROTECTED_MODE_CODE		0

; it is a label
header_start:
    ; `dd` means 'define double word'
    dd MULTIBOOT2_MAGIC_NUMBER
    dd PROTECTED_MODE_CODE
    dd header_end - header_start ; header length

    ; checksum
    dd 0x100000000 - (MULTIBOOT2_MAGIC_NUMBER + 0 + (header_end - header_start))

    ; required end tag
    ; `dw` means 'define word' (word = 16 bits on x86_64)
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
