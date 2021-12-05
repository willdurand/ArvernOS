/**
 * @file
 *
 * The main system timer.
 */
#ifndef TIME_TIMER_H
#define TIME_TIMER_H

#include <stdint.h>

/**
 * Initializes the system timer.
 */
void timer_init();

/**
 * Returns the number of seconds since the timer has been initialized.
 *
 * @return the number of seconds since the timer has been initialized
 */
uint64_t timer_uptime();

/**
 * Returns the number of microseconds since the timer has been initialized.
 *
 * @return the number of microseconds since the timer has been initialized
 */
uint64_t timer_uptime_microseconds();

#endif
