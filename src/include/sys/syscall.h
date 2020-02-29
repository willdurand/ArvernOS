/** @file */
#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>

#define INT_SYSCALL          "int $0x80"
#define NB_SYSCALLS          7
#define SYSCALL_TEST         1
#define SYSCALL_WRITE        2
#define SYSCALL_READ         3
#define SYSCALL_GETTIMEOFDAY 4
#define SYSCALL_OPEN         5
#define SYSCALL_CLOSE        6
#define SYSCALL_REBOOT       7

/**
 * Implements a syscall for testing purpose only. It takes a string as input
 * and prints it to the screen.
 *
 * @param s a string to print on the screen
 */
void test(const char* s);

/**
 * Implements the write syscall.
 *
 * @param fd a file descriptor ID
 * @param buf a buffer that contains the bytes to write
 * @param count the maximum number of bytes to write
 * @return the actual number of bytes write or -1 in case of an error
 */
ssize_t write(int fd, const void* buf, size_t count);

/**
 * Implements the read syscall. `read()` attempts to read up to `count` bytes
 * from file descriptor `fd` into the buffer starting at `buf`.
 *
 * @param fd a file descriptor ID
 * @param buf a buffer that receives the bytes read
 * @param count the maximum number of bytes to read
 * @return the actual number of bytes read or -1 in case of an error
 */
ssize_t read(int fd, void* buf, size_t count);

/**
 * Implements the gettimeofday syscall.
 */
int gettimeofday(struct timeval* p, void* z);

/**
 * Implements the open syscall.
 *
 * @param pathname the name of the file to open
 * @param flags creation or file status flags
 * @return a file descriptor, i.e. a small, nonnegative integer that is used in
 * subsequent system calls
 */
int open(const char* pathname, uint32_t flags, ...);

/**
 * Implements the close syscall.
 *
 * @param fd a file descriptor ID
 * @return zero on success, -1 otherwise
 */
int close(int fd);

/**
 * Implements the reboot syscall.
 *
 * @param command a reboot command
 * @return when `command` is `REBOOT_CMD_RESTART`, it does not return. -1 is
 * returned in case of an error, 0 otherwise.
 */
int reboot(int command);

#endif
