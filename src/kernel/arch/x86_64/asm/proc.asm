.global arch_task_switch
.global ret_from_fork
.global ret_from_fork_user
.global switch_to_ring3

.extern ret_to_user
.extern task_schedule_tail

.intel_syntax noprefix

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

  call task_schedule_tail

  call r12

# Entry point for a forked child's first scheduling. The child's kernel stack
# was seeded by `arch_task_create_child` with a copy of the parent's syscall
# frame, so all we need to do is re-enable preemption, make the syscall return
# value `0`, and fall through to the syscall return path (which pops the frame
# and `sysret`s into the parent's user context).
ret_from_fork_user:
  call task_schedule_tail
  xor eax, eax
  jmp ret_to_user

# Builds an `iretq` frame that drops the CPU to ring 3 (CPL=3), then performs
# the `iretq`. This never returns.
#
# rdi = entry point
# rsi = user stack top
# rdx = kernel stack top
switch_to_ring3:
  cli
  mov rsp, rdx
  push 0x23   # SS: user data
  push rsi    # RSP: user stack
  push 0x202  # RFLAGS
  push 0x2B   # CS: user code
  push rdi    # RIP: entry point
  iretq
