#include <arch/core.h>

#include <arch/kernel.h>
#include <board.h>
#include <core/isr.h>
#include <core/logging.h>
#include <core/mmio.h>
#include <stdint.h>

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

void isr_exception_handler(uint8_t type)
{
  WARN("isr_exception_handler invoked: %s", vectors[type]);

  arch_halt();
}
