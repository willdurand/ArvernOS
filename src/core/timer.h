/** @file */
#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <stdint.h>

/// The timer frequency.
#define TIMER_HZ 50

/**
 * Initializes the timer (clock).
 */
void timer_init();

/**
 * Returns the number of seconds since the timer has been initialized.
 *
 * @return the number of seconds since the timer has been initialized
 */
uint64_t timer_uptime();

/**
 * Returns the number of _ticks_ since the timer has been initialized,
 *
 * @return the number of _ticks_ since the timer has been initialized
 */
uint64_t timer_tick();

#endif
