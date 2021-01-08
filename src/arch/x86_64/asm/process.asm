global switch_tasks

switch_tasks:
	push rbp
	pushf

	; save rsp in old task
	mov [rdi + 0], rsp
	; load rsp from new task
	mov rsp, [rsi + 0]

	; update cr3
	mov rax, [rsi + 8]
	mov cr3, rax

	popf
	pop rbp

	ret
