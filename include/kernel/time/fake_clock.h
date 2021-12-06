/**
 * @file
 *
 * The fake clock is a fake implementation of a hardware clock (e.g., a
 * Real-Time Clock module). The clock is initialized at build time by injecting
 * the current build time (in nanoseconds). Note that this clock also requires
 * the system timer to update itself.
 *
 * This clock should be used when the target architecture/board does not have a
 * hardware clock. Use `CONFIG_USE_FAKE_CLOCK=1` to enable it.
 */
#ifndef TIME_FAKE_CLOCK_H
#define TIME_FAKE_CLOCK_H

#include <stdint.h>

/**
 * Initializes a fake clock.
 */
void fake_clock_init();

/**
 * Returns the current system time in microseconds.
 *
 * @return the current system time in microseconds
 */
uint64_t fake_clock_get_current_microseconds();

#endif
