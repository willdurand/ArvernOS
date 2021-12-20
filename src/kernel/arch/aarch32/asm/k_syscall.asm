.global syscall_handler

.include "src/kernel/arch/aarch32/asm/macro-defs.asm"

syscall_handler:
  kernel_entry swi
  // Read the SWI number, we expect 0 for a syscall. If it isn't 0, we call the
  // exception handler.
  // See: https://developer.arm.com/documentation/dui0203/f/Cihdaadc
  ldr r6, [lr, #-4]
  bic r6, r6, #0xff000000
  cmp r6, #0
  bne not_a_syscall
  bl isr_enable_interrupts
  ldr r6, =syscall_handlers // load syscall table pointer
  ldr r8, [r6, r7, lsl #2]  // r7 contains the syscall number
  blx r8
  bl isr_disable_interrupts
  str r0, [sp, #12]         // overwrite the value of r0 (with r0 a.k.a. the
                            // return value coming from the syscall) in the
                            // stack. It is currently #12 because we push {r2,
                            // r3} in `kernel_entry`.
  kernel_exit

not_a_syscall:
  mov r0, #2
  bl isr_exception_handler
  kernel_exit

// This is for testing purposes, it should not be here (this is part of the
// libc).
.global syscall
syscall:
  push {lr}
  mov r7, r0 // syscall number
  mov r0, r1 // 1st arg
  mov r1, r2 // 2nd arg
  mov r2, r3 // 3rd arg
  mov r3, r4 // 4th arg
  mov r4, r5 // 5th arg
  mov r5, r6 // 6th arg
  swi 0
  pop {lr}
  bx lr
