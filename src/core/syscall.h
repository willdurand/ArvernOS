#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <stdint.h>

#define NB_SYSCALLS   1
#define SYSCALL_TEST  0

typedef struct registers {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rsp;
} __attribute__((packed)) registers_t;

typedef void (*syscall_handler_t)(registers_t* registers);

void syscall_init();
void syscall_handler(registers_t* registers);

#endif
