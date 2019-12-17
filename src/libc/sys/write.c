#include <sys/syscall.h>

void write(char c) {
    __asm__(INT_SYSCALL : /* no output */ : "a"(SYSCALL_WRITE), "b"(c));
}
