; cf. https://intermezzos.github.io/book/multiboot-headers.html
section .multiboot_header

%define MULTIBOOT_HEADER_MAGIC 0xe85250d6
%define MULTIBOOT_HEADER_FLAGS 0

; it is a label
header_start:
	; `dd` means 'define double word'
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd header_end - header_start ; header length

    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; required end tag
	; `dw` means 'define word' (word = 16 bits on x86_64)
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
