/**
 * @file
 *
 * This file is a "proxy" header to ease compatibility with existing software.
 */
#ifndef UNISTD_H
#define UNISTD_H

#include <sys/syscall.h>

#ifndef __is_libk
extern char** environ;
#endif

#endif
