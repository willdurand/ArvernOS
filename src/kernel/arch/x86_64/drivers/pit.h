/**
 * @file
 * @see http://www.osdever.net/bkerndev/Docs/pit.htm
 *
 * Programmable Interval Timer (PIT).
 */
#ifndef DRIVERS_PIT_H
#define DRIVERS_PIT_H

#include <stdint.h>

/// The timer frequency.
#define PIT_FREQUENCY_HZ 100

/**
 * Initializes the PIT.
 */
void pit_init();

/**
 * Returns the number of seconds since the timer has been initialized.
 *
 * @return the number of seconds since the timer has been initialized
 */
uint64_t pit_uptime();

/**
 * Returns the number of _ticks_ since the timer has been initialized.
 *
 * @return the number of _ticks_ since the timer has been initialized
 */
uint64_t pit_tick();

#endif
