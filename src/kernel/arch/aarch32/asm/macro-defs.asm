.macro kernel_entry type
  .ifnc "\type", "swi"
  sub lr, lr, #4
  .endif
  srsdb sp!, #0x13
  cpsid if, #0x13
  push {r0-r3, r12, lr}
  and r3, sp, #4
  sub sp, sp, r3
  // r3 is the alignment padding (0 or 4) to keep SP 8-byte aligned. It is used
  // by `syscall_handler` to compute the offset of the saved r0 slot from the
  // current SP: offset = 8 + padding.
  push {r2, r3}
.endm

.macro kernel_exit
  pop {r2, r3}
  add sp, sp, r3
  pop {r0-r3, r12, lr}
  rfeia sp!
.endm
