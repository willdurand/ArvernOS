global syscall_handler

%define RSP0  0
%define CS    8
%define SS   16

syscall_handler:
  sti

%ifdef CONFIG_LINUX_COMPAT
  ; Save the current rsp,cs and ss to be used in the iret frame at the end.
  mov gs:[RSP0], rsp
  mov gs:[CS], cs
  mov gs:[SS], ss
%endif

  ; Save the return address, flags, base pointer for sysret.
  push rcx
  push r11
  push rbp

  ; Save callee-saved registers.
  push rbx
  push r12
  push r13
  push r14
  push r15

  ; Save the syscall params.
  push rdi
  push rsi
  push rdx
  push r10
  push r8
  push r9

  mov rcx, r10 ; fix 3rd syscall arg

  extern syscall_handlers
  call [rax * 8 + syscall_handlers]

  pop r9
  pop r8
  pop r10
  pop rdx
  pop rsi
  pop rdi

  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx

  pop rbp
  pop r11
  pop rcx

; This is needed because usermode has been broken recently (when the multi-
; tasking code has been introduced). Therefore, we only have kernel mode (ring
; 0). `sysret` always returns to ring 3 so we cannot use it when a syscall is
; done from ring 0 (which happens with Linux unmodified binaries). That's why
; we use `iretq`.
%ifdef CONFIG_LINUX_COMPAT
  push qword gs:[SS]
  push qword gs:[RSP0]
  push r11
  push qword gs:[CS]
  push rcx

  cli

  iretq
%else
  cli

  o64 sysret
%endif
