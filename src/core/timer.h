#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <stdint.h>

void timer_init(int freq);
uint32_t timer_tick();

#endif
