/** @file */
#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <core/isr.h>

/// This type represents a syscall handler.
typedef void (*syscall_handler_t)(registers_t* registers);

/**
 * Initializes the syscalls management.
 */
void syscall_init();

/**
 * This is the handler attached to the interrupt used by the syscalls. It is
 * called by the interrupt handler and responsible for finding the right
 * syscall handler to call.
 *
 * @param registers the registers passed to the handler
 */
void syscall_handler(registers_t* registers);

#endif
