.section ".text"

.global syscall
syscall:
  push {lr}
  mov r7, r0 // syscall number
  mov r0, r1 // 1st arg
  mov r1, r2 // 2nd arg
  mov r2, r3 // 3rd arg
  mov r3, r4 // 4th arg
  mov r4, r5 // 5th arg
  mov r5, r6 // 6th arg
  swi 0
  pop {lr}
  bx lr
