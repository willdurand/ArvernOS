// The `kernel_*` and `ventry` macros are based on Sergey Matyukevich's work,
// released under a MIT license.
//
// See: https://github.com/s-matyukevich/raspberry-pi-os/blob/master/docs/lesson03/rpi-os.md
.global vectors

.macro ventry_exc type
  .align 7
  mov x0, \type
  b call_exception_handler
.endm

.macro ventry_irq type
  .align 7
  b call_irq_handler
.endm

.macro kernel_entry
  sub	sp, sp, #256
  stp	x0, x1, [sp, #16 * 0]
  stp	x2, x3, [sp, #16 * 1]
  stp	x4, x5, [sp, #16 * 2]
  stp	x6, x7, [sp, #16 * 3]
  stp	x8, x9, [sp, #16 * 4]
  stp	x10, x11, [sp, #16 * 5]
  stp	x12, x13, [sp, #16 * 6]
  stp	x14, x15, [sp, #16 * 7]
  stp	x16, x17, [sp, #16 * 8]
  stp	x18, x19, [sp, #16 * 9]
  stp	x20, x21, [sp, #16 * 10]
  stp	x22, x23, [sp, #16 * 11]
  stp	x24, x25, [sp, #16 * 12]
  stp	x26, x27, [sp, #16 * 13]
  stp	x28, x29, [sp, #16 * 14]

  mrs	x22, elr_el1
  mrs	x23, spsr_el1

  stp	x30, x22, [sp, #16 * 15]
  str	x23, [sp, #16 * 16]
.endm

.macro kernel_exit
  ldr	x23, [sp, #16 * 16]
  ldp	x30, x22, [sp, #16 * 15]

  msr	elr_el1, x22
  msr	spsr_el1, x23

  ldp	x0, x1, [sp, #16 * 0]
  ldp	x2, x3, [sp, #16 * 1]
  ldp	x4, x5, [sp, #16 * 2]
  ldp	x6, x7, [sp, #16 * 3]
  ldp	x8, x9, [sp, #16 * 4]
  ldp	x10, x11, [sp, #16 * 5]
  ldp	x12, x13, [sp, #16 * 6]
  ldp	x14, x15, [sp, #16 * 7]
  ldp	x16, x17, [sp, #16 * 8]
  ldp	x18, x19, [sp, #16 * 9]
  ldp	x20, x21, [sp, #16 * 10]
  ldp	x22, x23, [sp, #16 * 11]
  ldp	x24, x25, [sp, #16 * 12]
  ldp	x26, x27, [sp, #16 * 13]
  ldp	x28, x29, [sp, #16 * 14]
  add	sp, sp, #256
  eret
.endm

.align 11
vectors:
  // EL1t: synchronous
  ventry_exc #0
  // EL1t: IRQ
  ventry_exc #1
  // EL1t: FIQ
  ventry_exc #2
  // EL1t: SError
  ventry_exc #3

  // EL1h: synchronous
  ventry_exc #4
  // EL1h: IRQ
  ventry_irq #5
  // EL1h: FIQ
  ventry_irq #6
  // EL1h: SError
  ventry_exc #7

  // 64bit EL0: synchronous
  ventry_exc #8
  // 64bit EL0: IRQ
  ventry_exc #9
  // 64bit EL0: FIQ
  ventry_exc #10
  // 64bit EL0: SError
  ventry_exc #11

  // 32bit EL0: synchronous
  ventry_exc #12
  // 32bit EL0: IRQ
  ventry_exc #13
  // 32bit EL0: FIQ
  ventry_exc #14
  // 32bit EL0: SError
  ventry_exc #15

call_exception_handler:
  kernel_entry
  bl isr_exception_handler
  b hang

call_irq_handler:
  kernel_entry
  bl isr_irq_handler
  kernel_exit

hang:
  wfe
  b hang
