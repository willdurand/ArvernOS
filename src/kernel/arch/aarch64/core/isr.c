#include "isr.h"
#include <board.h>
#include <core/logging.h>
#include <core/mmio.h>

static isr_handler_t handlers[16] = { 0 };

void isr_init()
{
  isr_enable_interrupts();
}

/**
 * Enables hardware interrupts.
 */
void isr_enable_interrupts()
{
  __asm__("msr daifclr, #2");
}

/**
 * Disables hardware interrupts.
 */
void isr_disable_interrupts()
{
  __asm__("msr daifset, #2");
}

void isr_irq_handler()
{
  uint32_t id = mmio_read(IRQ_PENDING_1);

  if (handlers[id] != 0) {
    isr_handler_t handler = handlers[id];
    handler();
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

void isr_register_handler(uint32_t id, isr_handler_t handler)
{
  handlers[id] = handler;
}
