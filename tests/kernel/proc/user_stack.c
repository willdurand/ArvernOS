#include <proc/user_stack.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <test.h>

#define STACK_SIZE 4096

static uint8_t buffer[STACK_SIZE];

int main()
{
  uintptr_t top = (uintptr_t)buffer + STACK_SIZE;

  describe("create_user_stack()");
  {
    char* argv[] = { "/bin/echo", "hello", "world", NULL };
    uintptr_t sp = create_user_stack(top, 3, argv, NULL, (uintptr_t)0x40001234);

    assert(sp != 0, "returns a non-zero stack pointer");
    assert((sp & 0xF) == 0, "returns a 16-byte aligned stack pointer");
    assert(sp < top, "stack pointer is below the stack top");

    uintptr_t* p = (uintptr_t*)sp;
    assert(p[0] == 3, "stores argc as the first word");
    assert(p[1] != 0 && strcmp((char*)p[1], "/bin/echo") == 0,
           "argv[0] points to the copied string");
    assert(p[2] != 0 && strcmp((char*)p[2], "hello") == 0,
           "argv[1] points to the copied string");
    assert(p[3] != 0 && strcmp((char*)p[3], "world") == 0,
           "argv[2] points to the copied string");
    assert(p[4] == 0, "argv is NULL-terminated");
    assert(p[5] == 0, "envp is an empty NULL-terminated list");
    assert(p[6] == AT_PAGESZ, "auxv starts with AT_PAGESZ");
    assert(p[7] == 4096, "AT_PAGESZ value is the page size");
    assert(p[8] == AT_ENTRY, "auxv contains AT_ENTRY");
    assert(p[9] == 0x40001234, "AT_ENTRY stores the entry point");
    assert(p[10] == AT_NULL, "auxv ends with AT_NULL");
    assert(p[11] == 0, "AT_NULL has a zero value");
  }
  end_describe();

  describe("create_user_stack() with envp");
  {
    char* argv[] = { "/bin/echo", NULL };
    char* envp[] = { "PATH=/bin", NULL };
    uintptr_t sp = create_user_stack(top, 1, argv, envp, (uintptr_t)0x4000);

    uintptr_t* p = (uintptr_t*)sp;
    assert(p[0] == 1, "stores argc");
    assert(strcmp((char*)p[1], "/bin/echo") == 0, "argv[0] is correct");
    assert(p[2] == 0, "argv is NULL-terminated");
    assert(p[3] != 0 && strcmp((char*)p[3], "PATH=/bin") == 0,
           "envp[0] points to the copied string");
    assert(p[4] == 0, "envp is NULL-terminated");
    assert(p[5] == AT_PAGESZ, "auxv follows the envp terminator");
    assert(p[7] == AT_ENTRY, "auxv contains AT_ENTRY");
    assert(p[8] == 0x4000, "AT_ENTRY stores the entry point");
  }
  end_describe();

  describe("create_user_stack() edge cases");
  {
    assert(create_user_stack(0, 0, NULL, NULL, 0) == 0,
           "returns 0 for a zero stack top");

    uintptr_t sp = create_user_stack(top, 0, NULL, NULL, 0x1000);
    assert(sp != 0, "handles an empty argv");
    assert((sp & 0xF) == 0, "aligned even with an empty argv");
    uintptr_t* p = (uintptr_t*)sp;
    assert(p[0] == 0, "stores argc=0");
    assert(p[1] == 0, "stores the argv NULL terminator");
  }
  end_describe();

  return test_summary();
}
