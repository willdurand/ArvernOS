.global _start

.extern main

.intel_syntax noprefix

.section .text
_start:
  # Prepare the arguments for `start_main()`. The System V ABI requires the
  # stack pointer to be 16-byte aligned before `call`, so index the argument
  # block instead of popping `argc` off the stack (which would leave `rsp`
  # misaligned at the call site).
  mov rdi, OFFSET main # `main()`
  mov rsi, [rsp]      # argc
  lea rdx, [rsp + 8]  # argv

  .extern start_main
  call start_main
