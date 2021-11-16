/**
 * @file
 *
 * This file is a "proxy" header to ease compatibility with existing software.
 */
#ifndef UNISTD_H
#define UNISTD_H

#include <sys/syscall.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifndef __is_libk
extern char* environ[];
#endif

#endif
