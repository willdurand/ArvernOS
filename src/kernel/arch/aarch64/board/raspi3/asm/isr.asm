// The `kernel_*` and `ventry` macros are based on Sergey Matyukevich's work,
// released under a MIT license.
//
// See: https://github.com/s-matyukevich/raspberry-pi-os/blob/master/docs/lesson03/rpi-os.md
.global vectors

.include "macro-defs.asm"

.macro ventry type
  .align 7
  b _ventry_\type
.endm

.macro call_exception_handler type, el
  mov x0, \type
  kernel_entry \el
  bl isr_exception_handler
  kernel_exit \el
.endm

.macro call_irq_handler type, el
  kernel_entry \el
  bl isr_irq_handler
  kernel_exit \el
.endm

.align 11
vectors:
  // EL1t: synchronous
  ventry 0
  // EL1t: IRQ
  ventry 1
  // EL1t: FIQ
  ventry 2
  // EL1t: SError
  ventry 3

  // EL1h: synchronous
  ventry 4
  // EL1h: IRQ
  ventry 5
  // EL1h: FIQ
  ventry 6
  // EL1h: SError
  ventry 7

  // 64bit EL0: synchronous
  ventry 8
  // 64bit EL0: IRQ
  ventry 9
  // 64bit EL0: FIQ
  ventry 10
  // 64bit EL0: SError
  ventry 11

  // 32bit EL0: synchronous
  ventry 12
  // 32bit EL0: IRQ
  ventry 13
  // 32bit EL0: FIQ
  ventry 14
  // 32bit EL0: SError
  ventry 15

_ventry_0:
  call_exception_handler #0 1
_ventry_1:
  call_exception_handler #1 1
_ventry_2:
  call_exception_handler #2 1
_ventry_3:
  call_exception_handler #3 1
_ventry_4:
  // TODO: We should not call `syscall_handler` here but we do because we
  // verify the syscall logic in kshell's `selftest`, i.e. we are using
  // syscalls in kernel mode.
  // call_exception_handler #4 1
  b syscall_handler
_ventry_5:
  call_irq_handler #5 1
_ventry_6:
  call_irq_handler #6 1
_ventry_7:
  call_exception_handler #7 1
_ventry_8:
  // This is defined in `src/kernel/arch/aarch64/asm/k_syscall.asm`.
  b syscall_handler
_ventry_9:
  call_irq_handler #9 0
_ventry_10:
  call_irq_handler #10 0
_ventry_11:
  call_exception_handler #11 0
_ventry_12:
  call_exception_handler #12 0
_ventry_13:
  call_exception_handler #13 0
_ventry_14:
  call_exception_handler #14 0
_ventry_15:
  call_exception_handler #15 0

hang:
  wfe
  b hang
