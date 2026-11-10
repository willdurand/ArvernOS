/** @file */
#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include <sys/types.h>

/// Wait for any child process.
#define WAIT_ANY (-1)

/**
 * Waits for a child process to change state.
 *
 * @param pid the child to wait for (`WAIT_ANY` for any child)
 * @param status where to store the child exit status (may be NULL)
 * @return the pid of the reaped child, or a negative error code
 */
pid_t waitpid(pid_t pid, int* status);

#endif
