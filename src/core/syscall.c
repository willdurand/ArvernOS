#include "syscall.h"
#include <stdio.h>
#include <stdint.h>

syscall_handler_t syscall_handlers[NB_SYSCALLS];

void syscall_handler(stack_t* stack);
void syscall_register_handler(uint8_t id, syscall_handler_t handler);

void syscall_test();

void syscall_init() {
    register_interrupt_handler(SYSCALL, syscall_handler);

    syscall_register_handler(SYSCALL_TEST, syscall_test);
}

void syscall_handler(stack_t* stack) {
    printf(
        "SYSCALL\n"
        "  instruction_pointer = 0x%X\n"
        "  code_segment        = 0x%X\n"
        "  cpu_flags           = 0x%X\n"
        "  stack_pointer       = 0x%X\n"
        "  stack_segment       = 0x%X\n",
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment
    );
}

void syscall_register_handler(uint8_t id, syscall_handler_t handler) {
    syscall_handlers[id] = handler;
}

void syscall_test() {
    printf("hello from syscall_test\n");
}
