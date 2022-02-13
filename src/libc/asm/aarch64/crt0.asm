.section ".text"

.global _start

_start:
  ldr x0, =main
  ldr x1, [sp], #8 // argc
  mov x2, sp       // argv
  bl start_main
