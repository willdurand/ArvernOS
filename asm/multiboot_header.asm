; This is a Multiboot-compliant header file in assembly code.
section .multiboot_header

%define MULTIBOOT2_MAGIC_NUMBER	0xe85250d6
%define PROTECTED_MODE_CODE		  0   ; architecture 0 (protected mode i386)
                                    ; architecture 4 (MIPS)

%define MULTIBOOT_HEADER_TAG_END            0 ; End Tag
%define MULTIBOOT_HEADER_TAG_FRAMEBUFFER    5 ; Framebuffer Tag (lets us pick a VGA mode by default)

%define MULTIBOOT_HEADER_TAG_OPTIONAL       1 ; Optional Tag (required for some tag types)

header_start:
    ; `dd` means 'define double word'
    dd MULTIBOOT2_MAGIC_NUMBER      ; magic number
    dd PROTECTED_MODE_CODE          ; architecture
    dd header_end - header_start    ; header length

    ; checksum
    dd 0x100000000 - (MULTIBOOT2_MAGIC_NUMBER + 0 + (header_end - header_start))

    ; framebuffer setup
framebuffer_tag_start:
    dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    dw MULTIBOOT_HEADER_TAG_OPTIONAL
    dd framebuffer_tag_end - framebuffer_tag_start
    dd 1024
    dd 768
    dd 32

framebuffer_tag_end:
    ; required end tag
    ; `dw` means 'define word' (word = 16 bits on x86_64)
    dw MULTIBOOT_HEADER_TAG_END ; type
    dw 0                        ; flags
    dd 0                        ; size
header_end:
