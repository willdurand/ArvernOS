global arch_task_switch
global ret_from_fork

arch_task_switch:
  push rbp
  push rbx
  push r12
  push r13
  push r14
  push r15
  pushfq

  mov [rdi + 0], rsp
  mov rsp, [rsi + 0]

  popfq
  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx
  pop rbp

  ret

ret_from_fork:
  pop r12

  extern task_schedule_tail
  call task_schedule_tail

  call r12

%ifdef CONFIG_LINUX_COMPAT
global linux_compat_start

linux_compat_start:
  mov rsp, rsi
  jmp rdi
%endif
