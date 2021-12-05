/**
 * @file
 *
 * The (system) clock is used to manage the system time.
 */
#ifndef TIME_CLOCK_H
#define TIME_CLOCK_H

#include <stdint.h>

/**
 * Initializes the system clock.
 */
void clock_init();

/**
 * Returns the current system time in microseconds.
 *
 * @return the current system time in microseconds
 */
uint64_t clock_get_current_microseconds();

#endif
