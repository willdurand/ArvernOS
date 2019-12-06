#include "syscall.h"
#include <kernel/panic.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

syscall_handler_t syscall_handlers[NB_SYSCALLS];

void syscall_register_handler(uint8_t id, syscall_handler_t handler);
void syscall_print_registers(registers_t* registers);

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
    printf("    (syscall_test) hello, %s!\n", registers->rbx);
}

void syscall_print_registers(registers_t* registers) {
    printf("registers:\n");
    printf("  rax=0x%X\n", registers->rax);
    printf("  rbx=0x%X\n", registers->rbx);
    printf("  rcx=0x%X\n", registers->rcx);
    printf("  rdx=0x%X\n", registers->rdx);
    printf("  rsi=0x%X\n", registers->rsi);
    printf("  rdi=0x%X\n", registers->rdi);
    printf("  rbp=0x%X\n", registers->rbp);
    printf("  r8 =0x%X\n", registers->r8);
    printf("  r9 =0x%X\n", registers->r9);
    printf("  r10=0x%X\n", registers->r10);
    printf("  r11=0x%X\n", registers->r11);
    printf("  r12=0x%X\n", registers->r12);
    printf("  r13=0x%X\n", registers->r13);
    printf("  r14=0x%X\n", registers->r14);
    printf("  r15=0x%X\n", registers->r15);
}
