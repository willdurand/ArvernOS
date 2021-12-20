.global vectors

.include "src/kernel/arch/aarch32/asm/macro-defs.asm"

.macro exc_handler type
exc_handler_\type:
  kernel_entry exc
  mov r0, #\type
  bl isr_exception_handler
  b hang
.endm

.macro irq_handler type
irq_handler_\type:
  kernel_entry irq
  bl isr_irq_handler
  kernel_exit
.endm

.align 5
vectors:
  ldr pc, ventry_addr_0
  ldr pc, ventry_addr_1
  ldr pc, ventry_addr_2
  ldr pc, ventry_addr_3
  ldr pc, ventry_addr_4
  ldr pc, ventry_addr_5
  ldr pc, ventry_addr_6
  ldr pc, ventry_addr_7

ventry_addr_0: .word start
ventry_addr_1: .word exc_handler_1
ventry_addr_2: .word syscall_handler
ventry_addr_3: .word exc_handler_3
ventry_addr_4: .word exc_handler_4
ventry_addr_5: .word exc_handler_5
ventry_addr_6: .word irq_handler_6
ventry_addr_7: .word exc_handler_7

exc_handler 1
exc_handler 3
exc_handler 4
exc_handler 5
irq_handler 6
exc_handler 7

hang:
  b hang
