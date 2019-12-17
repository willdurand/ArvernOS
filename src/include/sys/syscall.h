/** @file */
#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#define INT_SYSCALL   "int $0x80"
#define NB_SYSCALLS   2
#define SYSCALL_TEST  1
#define SYSCALL_WRITE 3

/**
 * Implements a syscall for testing purpose only. It takes a string as input
 * and prints it to the screen.
 *
 * @param s a string to print on the screen
 */
void test(const char* s);

/**
 */
void write(char c);

#endif
