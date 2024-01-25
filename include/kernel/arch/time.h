/**
 * @file
 *
 * Architecture-specific code for time management.
 */
#ifndef ARCH_TIME_H
#define ARCH_TIME_H

#include <stdint.h>
#include <time.h>

#define SEC_TO_MICROSEC(s)  ((s) * 1000000)
#define MICROSEC_TO_SEC(us) ((us) / 1000000)

/**
 * Initializes the system timer.
 */
void arch_timer_init();

/**
 * Returns the number of microseconds since the timer has been initialized.
 *
 * @return the number of microseconds since the timer has been initialized
 */
uint64_t arch_timer_uptime_microseconds();

/**
 * Initializes the system clock.
 */
void arch_clock_init();

/**
 * Returns the current time in microseconds.
 */
uint64_t arch_clock_get_current_microseconds();

#endif
