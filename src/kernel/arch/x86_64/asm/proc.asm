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
  extern task_schedule_tail
  call task_schedule_tail
  cli
  iretq
