#include <arch/time.h>

#include <board.h>
#include <core/isr.h>
#include <core/mmio.h>
#include <time/fake_clock.h>

#define FREQ_1MHZ 1000000

void arch_timer_callback();

static uint32_t current_value_1 = 0;

void arch_timer_init()
{
  isr_register_handler(SYSTEM_TIMER_IRQ_1, arch_timer_callback);
  mmio_write(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);

  current_value_1 = mmio_read(TIMER_CLO);
  current_value_1 += FREQ_1MHZ / 1000;

  mmio_write(TIMER_CS, TIMER_CS_M1);
  mmio_write(TIMER_C1, current_value_1);
}

void arch_timer_callback()
{
  // TODO: It would probably be a good idea to handle overflows because we are
  // using a uint32_t for the current value.
  current_value_1 += FREQ_1MHZ / 1000;

  mmio_write(TIMER_CS, TIMER_CS_M1);
  mmio_write(TIMER_C1, current_value_1);
}

uint64_t arch_timer_uptime_microseconds()
{
  uint32_t hi = mmio_read(TIMER_CHI);
  uint32_t lo = mmio_read(TIMER_CLO);

  while (hi != mmio_read(TIMER_CHI)) {
    hi = mmio_read(TIMER_CHI);
    lo = mmio_read(TIMER_CLO);
  }

  return ((uint64_t)hi << 32) | lo;
}

void arch_clock_init()
{
  fake_clock_init();
}

uint64_t arch_clock_get_current_microseconds()
{
  return fake_clock_get_current_microseconds();
}
