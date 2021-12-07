.section ".text"

.global syscall

syscall:
  mov x8, x0 // syscall number
  mov x0, x1 // 1st arg
  mov x1, x2 // 2nd arg
  mov x2, x3 // 3rd arg
  mov x3, x4 // 4th arg
  mov x4, x5 // 5th arg
  mov x5, x6 // 6th arg
  svc #0
  ret
