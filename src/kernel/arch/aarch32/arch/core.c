#include <arch/core.h>

#include <arch/kernel.h>
#include <board.h>
#include <core/isr.h>
#include <core/logging.h>
#include <core/mmio.h>
#include <proc/task.h>
#include <stdint.h>

// Invalid memory reference.
#define SIGSEGV 11

// CPSR mode field: `0x1F` is the mask and `0x10` is USR mode.
#define CPSR_MODE_MASK 0x1F
#define CPSR_MODE_USR  0x10

static const char* vectors[8] = {
  "reset",
  "undefined instruction",
  "software interrupt",
  "prefetch abort",
  "data abort",
  "unused",
  "irq",
  "fast irq",
};

void arch_isr_init()
{
  // The vector table, which is required for interrupts to work, is loaded in
  // `src/kernel/arch/aarch32/board/raspi2/asm/boot.asm` so there is nothing to
  // do here.
}

void arch_isr_enable_interrupts()
{
  __asm__("cpsie i");
}

void arch_isr_disable_interrupts()
{
  __asm__("cpsid i");
}

void isr_irq_handler()
{
  uint32_t id = mmio_read(IRQ_PENDING_1);
  isr_handler_t handler = isr_get_handler(id);

  if (handler != 0) {
    handler(NULL);
  } else {
    WARN("unknown pending IRQ: id=%ld", id);
  }
}

void isr_exception_handler(uint8_t type, uint32_t spsr)
{
  // An exception taken from user mode is not a kernel bug: terminate the task
  // with exit code set to SIGSEGV instead of halting the kernel.
  if ((spsr & CPSR_MODE_MASK) == CPSR_MODE_USR) {
    task_exit(SIGSEGV);
    return;
  }

  WARN("isr_exception_handler invoked: %s", vectors[type]);

  arch_halt();
}
