global syscall_handler

syscall_handler:
  push rsp     ; save user stack
  push r11     ; save the flags
  push rcx     ; save the return address
  mov rcx, r10 ; fix 3rd syscall arg
  sti

  extern syscall_handlers
  mov r10, [rax * 8 + syscall_handlers]
  call r10

  cli
  pop rcx
  pop r11
  pop rsp ; restore user stack
  o64 sysret
