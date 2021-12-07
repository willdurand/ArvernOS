#include <arch/core.h>

#include <board.h>
#include <core/isr.h>
#include <core/logging.h>
#include <core/mmio.h>

void arch_isr_init()
{
  // nothing to do
}

void arch_isr_enable_interrupts()
{
  __asm__("msr daifclr, #2");
}

void arch_isr_disable_interrupts()
{
  __asm__("msr daifset, #2");
}

void isr_irq_handler()
{
  uint32_t id = mmio_read(IRQ_PENDING_1);
  isr_handler_t handler = isr_get_handler(id);

  if (handler != 0) {
    handler(NULL);
  } else {
    DEBUG("unknown pending IRQ: id=%ld", id);
  }
}

void isr_exception_handler(uint8_t type)
{
  ERROR("exception handler invoked: type=%d", type);

  while (1) {
    ;
  }
}
