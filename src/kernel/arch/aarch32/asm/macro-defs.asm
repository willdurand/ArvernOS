.macro kernel_entry type
  .ifnc "\type", "swi"
  sub lr, lr, #4
  .endif
  srsdb sp!, #0x13
  cpsid if, #0x13
  push {r0-r3, r12, lr}
  and r3, sp, #4
  sub sp, sp, r3
  push {r2, r3}
.endm

.macro kernel_exit
  pop {r2, r3}
  add sp, sp, r3
  pop {r0-r3, r12, lr}
  rfeia sp!
.endm
