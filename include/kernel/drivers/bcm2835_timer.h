/**
 * @file
 * @see https://wiki.osdev.org/BCM_System_Timer
 */
#ifndef DRIVERS_BCM2835_TIMER_H
#define DRIVERS_BCM2835_TIMER_H

#include <stdint.h>

/**
 * Initializes the BCM2835 system timer (currently, only the timer 1).
 *
 * @param mmio_base the MMIO base address to use to get access to the System
 * Timer registers
 * @param callback a function that gets called every time the timer fires an
 * interrupt
 */
void bcm2835_timer_init(uintptr_t mmio_base, void (*callback)());

/**
 * Returns the timer uptime in microseconds.
 *
 * @return the timer uptime in microseconds
 */
uint64_t bcm2835_timer_uptime_microseconds();

#endif
