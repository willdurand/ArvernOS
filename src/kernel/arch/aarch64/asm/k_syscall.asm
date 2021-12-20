.global syscall_handler

.macro kernel_entry
  sub sp, sp, #256
  stp x0, x1, [sp, #16 * 0]
  stp x2, x3, [sp, #16 * 1]
  stp x4, x5, [sp, #16 * 2]
  stp x6, x7, [sp, #16 * 3]
  stp x8, x9, [sp, #16 * 4]
  stp x10, x11, [sp, #16 * 5]
  stp x12, x13, [sp, #16 * 6]
  stp x14, x15, [sp, #16 * 7]
  stp x16, x17, [sp, #16 * 8]
  stp x18, x19, [sp, #16 * 9]
  stp x20, x21, [sp, #16 * 10]
  stp x22, x23, [sp, #16 * 11]
  stp x24, x25, [sp, #16 * 12]
  stp x26, x27, [sp, #16 * 13]
  stp x28, x29, [sp, #16 * 14]

  add x21, sp, #256
  mrs x22, elr_el1
  mrs x23, spsr_el1

  stp x30, x21, [sp, #16 * 15]
  str x23, [sp, #16 * 16]
.endm

.macro kernel_exit
  ldr x23, [sp, #16 * 16]
  ldp x30, x21, [sp, #16 * 15]

  msr elr_el1, x22
  msr spsr_el1, x23

  ldp x0, x1, [sp, #16 * 0]
  ldp x2, x3, [sp, #16 * 1]
  ldp x4, x5, [sp, #16 * 2]
  ldp x6, x7, [sp, #16 * 3]
  ldp x8, x9, [sp, #16 * 4]
  ldp x10, x11, [sp, #16 * 5]
  ldp x12, x13, [sp, #16 * 6]
  ldp x14, x15, [sp, #16 * 7]
  ldp x16, x17, [sp, #16 * 8]
  ldp x18, x19, [sp, #16 * 9]
  ldp x20, x21, [sp, #16 * 10]
  ldp x22, x23, [sp, #16 * 11]
  ldp x24, x25, [sp, #16 * 12]
  ldp x26, x27, [sp, #16 * 13]
  ldp x28, x29, [sp, #16 * 14]
  add  sp, sp, #256
  eret
.endm

syscall_handler:
  kernel_entry
  mrs x25, esr_el1            // read the syndrome register
  lsr x24, x25, #26           // exception class
  cmp x24, #0x15              // SVC in 64-bit state

  adr x27, syscall_handlers   // load syscall table pointer
  uxtw x26, x8                // syscall number
  bl isr_enable_interrupts
  ldr x16, [x27, x26, lsl #3] // address in the syscall table
  blr x16                     // call syscall
  b ret_from_syscall

ret_from_syscall:
  bl isr_disable_interrupts
  str x0, [sp, #0]            // returned x0
  kernel_exit

// This is for testing purposes, it should not be here (this is part of the
// libc).
.global syscall
syscall:
  mov x8, x0 // syscall number
  mov x0, x1 // 1st arg
  mov x1, x2 // 2nd arg
  mov x2, x3 // 3rd arg
  mov x3, x4 // 4th arg
  mov x4, x5 // 5th arg
  mov x5, x6 // 6th arg
  svc #0
  ret
