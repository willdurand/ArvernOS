#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <core/isr.h>

typedef void (*syscall_handler_t)(registers_t* registers);

void syscall_init();
void syscall_handler(registers_t* registers);

#endif
