; cf. https://intermezzos.github.io/book/multiboot-headers.html
section .multiboot_header

; it is a label
header_start:
	; `dd` means 'define double word'
	dd 0xe85250d6                ; magic number
	dd 0                         ; protected mode code
	dd header_end - header_start ; header length

    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; required end tag
	; `dw` means 'define word' (word = 16 bits on x86_64)
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:
