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
    printf("  rax=%#x\n", registers->rax);
    printf("  rbx=%#x\n", registers->rbx);
    printf("  rcx=%#x\n", registers->rcx);
    printf("  rdx=%#x\n", registers->rdx);
    printf("  rsi=%#x\n", registers->rsi);
    printf("  rdi=%#x\n", registers->rdi);
    printf("  rbp=%#x\n", registers->rbp);
    printf("   r8=%#x\n", registers->r8);
    printf("   r9=%#x\n", registers->r9);
    printf("  r10=%#x\n", registers->r10);
    printf("  r11=%#x\n", registers->r11);
    printf("  r12=%#x\n", registers->r12);
    printf("  r13=%#x\n", registers->r13);
    printf("  r14=%#x\n", registers->r14);
    printf("  r15=%#x\n", registers->r15);
}
