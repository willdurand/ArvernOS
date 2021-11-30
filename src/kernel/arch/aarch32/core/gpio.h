#ifndef CORE_GPIO_H
#define CORE_GPIO_H

#include <stdint.h>

void gpio_configure_function(uint32_t addr, uint8_t pin, uint8_t func);

#endif
