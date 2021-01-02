global syscall_handler

syscall_handler:
  push rcx     ; save the return address
  push r11     ; save the flags
  mov rcx, r10 ; fix 3rd syscall arg
  sti

  extern syscall_handlers
  call [rax * 8 + syscall_handlers]

  cli
  pop r11
  pop rcx
  o64 sysret
