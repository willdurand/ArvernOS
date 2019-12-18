/** @file */
#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#define INT_SYSCALL   "int $0x80"
#define NB_SYSCALLS   3
#define SYSCALL_TEST  1
#define SYSCALL_WRITE 2
#define SYSCALL_READ  3

/**
 * Implements a syscall for testing purpose only. It takes a string as input
 * and prints it to the screen.
 *
 * @param s a string to print on the screen
 */
void test(const char* s);

/**
 * Implements the write (to screen) syscall.
 *
 * @param c the character to print on the screen
 */
void write(char c);

/**
 */
char read();

#endif
