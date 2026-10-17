global syscall_handler
global ret_to_user
global syscall_frame

extern tss

; System call entry point.
;
; `syscall` does not switch the stack, so we stash the user stack pointer and
; load the current task's kernel stack from `tss.rsp0`, the same value the CPU
; loads for interrupts taken from ring 3. We intentionally avoid `swapgs`: the
; kernel never uses the `gs` segment outside of a syscall, so keeping `gs`
; untouched means user mode always observes the same `gs` value it entered with
; (and fork children resume correctly regardless of the context they are first
; scheduled from). Interrupts are masked on entry via `IA32_SFMASK` until the
; frame is fully built.
;
; The saved frame (from low to high addresses) is:
;   r15, r14, r13, r12, r10, r9, r8, rsi, rdi, rdx, rbx, rbp,
;   rip (rcx), rflags (r11), user_rsp
;
; The `user_rsp` slot is what a forked child needs in order to resume the
; parent's user context via the `ret_to_user` epilogue.
syscall_handler:
  mov [rel syscall_user_rsp], rsp
  mov rsp, [rel tss + 4] ; tss.rsp0

  push qword [rel syscall_user_rsp] ; user_rsp
  push r11                          ; rflags
  push rcx                          ; rip (return address)

  push rbp
  push rbx
  push rdx
  push rdi
  push rsi
  push r8
  push r9
  push r10
  push r12
  push r13
  push r14
  push r15

  ; Remember where this frame starts so that a child process can copy the parent's
  ; user context if needed.
  mov [rel syscall_frame], rsp

  sti

  mov rcx, r10 ; fix 3rd syscall arg

  extern syscall_handlers
  call [rax * 8 + syscall_handlers]

ret_to_user:
  cli

  pop r15
  pop r14
  pop r13
  pop r12
  pop r10
  pop r9
  pop r8
  pop rsi
  pop rdi
  pop rdx
  pop rbx
  pop rbp

  pop rcx                ; rip
  pop r11                ; rflags
  pop rsp                ; user_rsp

  o64 sysret

section .bss
align 8
syscall_user_rsp: resq 1
syscall_frame:    resq 1
