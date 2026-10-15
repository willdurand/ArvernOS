.section ".text"

.global syscall
syscall:
  // r4-r7 are callee-saved registers: we must save them before we reuse them
  // to pass the syscall number and the arguments, and restore them afterwards.
  push {r4, r5, r6, r7, lr}
  mov r7, r0 // syscall number
  mov r0, r1 // 1st arg
  mov r1, r2 // 2nd arg
  mov r2, r3 // 3rd arg
  mov r3, r4 // 4th arg
  mov r4, r5 // 5th arg
  mov r5, r6 // 6th arg
  swi 0
  pop {r4, r5, r6, r7, lr}
  bx lr
