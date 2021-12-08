.section ".text"

.global _start

_start:
  ldr r0, =main
  bl start_main
