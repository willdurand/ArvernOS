global _start

extern main

section .text
_start:
	xor ebp, ebp ; Clear the frame pointer.

	; Prepare the arguments for `start_main()`.
	mov rdi, main      ; `main()`
	mov rsi, [rsp]     ; `argc`
	mov rdx, [rsp + 4] ; `argv`

	extern start_main
	call start_main
