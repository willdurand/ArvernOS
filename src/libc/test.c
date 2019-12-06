#include <test.h>

void test() {
    __asm__("int $80" :: "a"(22));
}
