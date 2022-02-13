global _start

extern main

section .text
_start:
  ; Prepare the arguments for `start_main()`.
  mov rdi, main ; `main()`
  pop rsi       ; argc
  mov rdx, rsp  ; argv

  extern start_main
  call start_main
