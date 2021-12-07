global syscall

syscall:
  mov rax, rdi      ; syscall number
  mov rdi, rsi      ; 1st arg
  mov rsi, rdx      ; 2nd arg
  mov rdx, rcx      ; 3rd arg
  mov r10, r8       ; 4th arg
  mov r8, r9        ; 5th arg
  mov r9, [rsp + 8] ; 6th arg
  syscall
  ret
