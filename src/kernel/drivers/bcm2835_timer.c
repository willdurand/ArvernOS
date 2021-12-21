#include <drivers/bcm2835_timer.h>

#include <core/isr.h>
#include <core/mmio.h>
#include <stdint.h>

#define FREQ_1MHZ 1000000

#define ENABLE_IRQS_1      (_mmio_base + 0x0000B210)
#define SYSTEM_TIMER_IRQ_1 (1 << 1)

#define TIMER_CS    (_mmio_base + 0x00003000)
#define TIMER_CLO   (_mmio_base + 0x00003004)
#define TIMER_CHI   (_mmio_base + 0x00003008)
#define TIMER_C1    (_mmio_base + 0x00003010)
#define TIMER_CS_M1 (1 << 1)

void bcm2835_timer_callback();

static uintptr_t _mmio_base = 0;
static void (*_callback)();

void bcm2835_timer_init(uintptr_t mmio_base, void (*callback)())
{
  _mmio_base = mmio_base;
  _callback = callback;

  isr_register_handler(SYSTEM_TIMER_IRQ_1, bcm2835_timer_callback);
  mmio_write(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);

  uint32_t value = mmio_read(TIMER_CLO);

  mmio_write(TIMER_CS, TIMER_CS_M1);
  mmio_write(TIMER_C1, value + (FREQ_1MHZ / 100));
}

uint64_t bcm2835_timer_uptime_microseconds()
{
  // CLO and CHI form a 64-bit free-running counter, which increases by itself
  // at a rate of 1 MHz.
  uint32_t hi = mmio_read(TIMER_CHI);
  uint32_t lo = mmio_read(TIMER_CLO);

  while (hi != mmio_read(TIMER_CHI)) {
    hi = mmio_read(TIMER_CHI);
    lo = mmio_read(TIMER_CLO);
  }

  return ((uint64_t)hi << 32) | lo;
}

void bcm2835_timer_callback()
{
  mmio_write(TIMER_CS, TIMER_CS_M1);

  uint32_t value = mmio_read(TIMER_CLO);

  mmio_write(TIMER_CS, TIMER_CS_M1);
  mmio_write(TIMER_C1, value + (FREQ_1MHZ / 100));

  // Call this function after we have rearmed the timer to make sure it will
  // fire again.
  _callback();
}
