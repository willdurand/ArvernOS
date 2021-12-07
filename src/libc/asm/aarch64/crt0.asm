.section ".text"

.global _start

_start:
  ldr x0, main
  bl start_main
