.global syscall_handler

.include "macro-defs.asm"

syscall_handler:
  kernel_entry 0
  mrs x25, esr_el1            // read the syndrome register
  lsr x24, x25, #26           // exception class
  cmp x24, #0x15              // SVC in 64-bit state
  b.ne not_a_syscall

  adr x27, syscall_handlers   // load syscall table pointer
  uxtw x26, x8                // syscall number
  bl isr_enable_interrupts
  ldr x16, [x27, x26, lsl #3] // address in the syscall table
  blr x16                     // call syscall
  b ret_from_syscall

ret_from_syscall:
  bl isr_disable_interrupts
  str x0, [sp, #0]            // returned x0
  kernel_exit 0

not_a_syscall:
  mov x0, #81
  bl isr_exception_handler
  kernel_exit 0

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
