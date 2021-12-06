.global vectors

.macro ventry type label
  .align 7
  mov x0, \type
  b \label
.endm

.align 11
vectors:
  // EL1t: synchronous
  ventry #0 exception_handler
  // EL1t: IRQ
  ventry #1 exception_handler
  // EL1t: FIQ
  ventry #2 exception_handler
  // EL1t: SError
  ventry #3 exception_handler
  // TODO: add the other vector entries (for EL1h, EL0 64bit and 32bit)

exception_handler:
  eret
