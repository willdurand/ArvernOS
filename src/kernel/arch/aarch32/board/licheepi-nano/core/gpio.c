#include "gpio.h"
#include <core/mmio.h>

#define GPIO_CFG_OFFSET(pin) ((((pin)&0x1F) & 0x7) << 2)

void gpio_configure_function(uint32_t addr, uint8_t pin, uint8_t func)
{
  uint32_t val = mmio_read(addr);

  val &= ~(0xF << GPIO_CFG_OFFSET(pin));
  val |= ((func & 0x7) << GPIO_CFG_OFFSET(pin));

  mmio_write(addr, val);
}
