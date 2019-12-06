#include <sys/syscall.h>

void test(const char* s) {
    __asm__("int $80" : /* no output */ : "a"(SYSCALL_TEST), "b"(s));
}
