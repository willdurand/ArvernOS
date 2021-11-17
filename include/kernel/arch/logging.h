/**
 * @file
 *
 * Architecture-specific implementation to output logs.
 */
#ifndef ARCH_LOGGING_H
#define ARCH_LOGGING_H

/**
 * This function is used to output logs somewhere. In most cases, a serial line
 * will be used but it is up to the implementers, depending on the constraints.
 *
 * @param c a character to print
 * @param arg optional arguments. This should be `NULL` all the time.
 */
void arch_logging_stream_output(char c, void* arg);

#endif
