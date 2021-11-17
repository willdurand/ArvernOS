#ifndef ARCH_LOGGING_H
#define ARCH_LOGGING_H

/**
 * Architecture-specific code should implement this function to output logs
 * somewhere. In most cases, a serial line will be used.
 *
 * @param c a character to print
 * @param arg optional arguments. This should be `NULL` all the time.
 */
void arch_logging_stream_output(char c, void* arg);

#endif
