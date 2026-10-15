.global ret_from_fork
.global arch_task_switch
.global switch_to_el0

.include "macro-defs.asm"

ret_from_fork:
  bl task_schedule_tail
  cbz x19, ret_to_user // not a kernel thread
  blr x19
ret_to_user:
  bl isr_disable_interrupts
  kernel_exit 0

arch_task_switch:
  mov x10, #0
  add x8, x0, x10
  mov x9, sp
  stp x19, x20, [x8], #16 // store callee-saved registers
  stp x21, x22, [x8], #16
  stp x23, x24, [x8], #16
  stp x25, x26, [x8], #16
  stp x27, x28, [x8], #16
  stp x29, x9, [x8], #16
  str x30, [x8]
  add x8, x1, x10
  ldp x19, x20, [x8], #16 // restore callee-saved registers
  ldp x21, x22, [x8], #16
  ldp x23, x24, [x8], #16
  ldp x25, x26, [x8], #16
  ldp x27, x28, [x8], #16
  ldp x29, x9, [x8], #16
  ldr x30, [x8]
  mov sp, x9
  ret

// Drops the CPU to EL0 and starts executing the program at the given entry
// point (x0), with the user stack top (x1).
switch_to_el0:
  msr sp_el0,   x1
  msr elr_el1,  x0
  ldr x2,       =0x00000000 // EL0t, DAIF 0
  msr spsr_el1, x2
  eret
