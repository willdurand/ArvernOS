.section ".text"

.global _start

_start:
  ldr x0, =main
  ldr x1, [sp]     // argc
  add x2, sp, #8   // argv
  bl start_main
