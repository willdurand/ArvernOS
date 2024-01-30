.intel_syntax noprefix

.global "syscall_handler"

syscall_handler:
  sti

  push rcx     // save the return address
  push r11     // save the flags
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

  mov rcx, r10 // fix 3rd syscall arg

  call [rax * 8 + syscall_handlers]

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
  pop r11
  pop rcx

  cli
  sysret
