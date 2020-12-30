global _start

extern main

section .text
_start:
	xor ebp, ebp ; Clear the frame pointer.

	; Prepare the arguments for `start_main()`.
	mov rdi, main ; `main()`
	pop rsi       ; `argc`
	pop rdx       ; `argv`

	extern start_main
	call start_main
