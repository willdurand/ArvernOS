#include <arch/core.h>

#include <arch/kernel.h>
#include <board.h>
#include <core/isr.h>
#include <core/logging.h>
#include <core/mmio.h>
#include <proc/task.h>

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
  uint64_t esr = 0, far = 0, spsr = 0;
  __asm__("mrs %0, esr_el1" : "=r"(esr) : /* no input */);
  __asm__("mrs %0, far_el1" : "=r"(far) : /* no input */);
  __asm__("mrs %0, spsr_el1" : "=r"(spsr) : /* no input */);

  // A synchronous exception taken from user mode is not a kernel bug:
  // terminate the task with exit code set to SIGSEGV instead of halting the
  // kernel.
  if ((spsr & 0xf) == 0) {
    WARN("user fault: type=%d esr_el1=0x%lx far_el1=0x%lx, killing it",
         type,
         esr,
         far);
    task_exit(SIGSEGV);
    return;
  }

  WARN("exception handler invoked: type=%d esr_el1=0x%lx far_el1=0x%lx",
       type,
       esr,
       far);

  arch_halt();
}
