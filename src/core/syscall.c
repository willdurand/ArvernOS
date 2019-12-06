#include "syscall.h"
#include <kernel/panic.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

syscall_handler_t syscall_handlers[NB_SYSCALLS];

void syscall_register_handler(uint8_t id, syscall_handler_t handler);

void syscall_test(registers_t* registers);

void syscall_init() {
    syscall_register_handler(SYSCALL_TEST, syscall_test);
}

void syscall_register_handler(uint8_t id, syscall_handler_t handler) {
    syscall_handlers[id] = handler;
}

void syscall_handler(registers_t* registers) {
    syscall_handler_t handler = syscall_handlers[registers->rax];

    if (handler != 0) {
        handler(registers);
        return;
    }

    PANIC("Received unimplemented syscall: %d\n", registers->rax);
}

void syscall_test(registers_t* registers) {
    printf("hello from syscall_test: %c\n", registers->rbx);
}
