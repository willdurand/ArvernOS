global long_mode_start
extern kmain

section .text
bits 64
long_mode_start:
	call kmain

	; should not happen
	hlt
