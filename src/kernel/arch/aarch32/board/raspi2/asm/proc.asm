.global arch_task_switch
.global ret_from_fork
.global switch_to_usr

// r0: prev task
// r1: next task
arch_task_switch:
  mov r2, r0
  stmia r2!, {r4 - r11}
  mrs r3, cpsr
  str r3, [r2], #4
  str sp, [r2], #4
  str lr, [r2], #4

  mov r2, r1
  ldmia r2!, {r4 - r11}
  ldr r3, [r2]
  msr cpsr, r3
  add r2, r2, #4
  ldr sp, [r2]
  ldr pc, [r2, #4]!

ret_from_fork:
  bl task_schedule_tail
  blx r4
  // A kernel thread that returns from its entry point must not fall off the
  // end of this file: terminate it instead.
  mov r0, #0
  bl task_exit

// Drops the CPU to USR mode and starts executing the program at the given
// entry point. From SVC mode we set the user stack pointer (`sp_usr`), program
// the SPSR for the return (`0x10` = USR mode with I/F enabled) and return
// through `movs pc, lr`, which restores CPSR from SPSR and branches to the
// entry point.
//
// The MMU is disabled on this target so user and kernel memory share a single
// physical address space.
//
// r0: entry point
// r1: user stack top
switch_to_usr:
  msr sp_usr, r1
  mov r2, #0x10
  msr spsr_cxsf, r2
  mov lr, r0
  movs pc, lr
