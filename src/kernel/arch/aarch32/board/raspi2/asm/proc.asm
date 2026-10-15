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

// r0: entry point
// r1: user stack top
switch_to_usr:
  msr sp_usr, r1
  mov r2, #0x10
  msr spsr, r2
  mov lr, r0
  movs pc, lr
