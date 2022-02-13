.section ".text"

.global _start

_start:
  ldr r0, =main
  pop {r1}       // argc
  mov r2, sp     // argv
  bl start_main
