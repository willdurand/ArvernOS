#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <core/isr.h>

#define NB_SYSCALLS   1
#define SYSCALL_TEST  1

typedef void (*syscall_handler_t)(stack_t* stack);

void syscall_init();

#endif
