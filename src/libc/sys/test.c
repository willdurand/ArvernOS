#include <sys/syscall.h>

void test(char c) {
    __asm__("int $80" : : "n"(SYSCALL_TEST), "a"(c));
}
