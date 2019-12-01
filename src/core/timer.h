#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <stdint.h>

#define TIMER_HZ 50

void timer_init();
uint32_t timer_uptime();
uint32_t timer_tick();

#endif
