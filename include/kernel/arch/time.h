/**
 * @file
 *
 * Architecture-specific code for time management.
 */
#ifndef ARCH_TIME_H
#define ARCH_TIME_H

#include <time.h>

/**
 * Returns the time of day.
 */
void arch_gettimeofday(struct timeval* p, void* z);

#endif
