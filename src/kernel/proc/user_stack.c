#include <proc/user_stack.h>

#include <string.h>

static uintptr_t push_string(uintptr_t* sp, const char* str)
{
  size_t len = strlen(str) + 1;
  *sp -= len;
  memcpy((void*)*sp, str, len);
  return *sp;
}

uintptr_t create_user_stack(uintptr_t stack_top,
                            int argc,
                            char* const argv[],
                            char* const envp[],
                            uintptr_t entry)
{
  if (stack_top == 0 || argc < 0) {
    return 0;
  }

  // Align the top of the stack. We reserve 16 bytes of padding below the
  // strings so that there is always room even for an empty argv/envp.
  uintptr_t sp = stack_top & ~((uintptr_t)STACK_ALIGNMENT - 1);

  int envc = 0;
  if (envp != NULL) {
    while (envp[envc] != NULL) {
      envc++;
    }
  }

  if (argc > MAX_STACK_ARGS || envc > MAX_STACK_ARGS) {
    return 0;
  }

  // Copy the argument and environment strings, recording their addresses.
  uintptr_t argv_ptrs[MAX_STACK_ARGS];
  for (int i = 0; i < argc; i++) {
    const char* arg = (argv != NULL && argv[i] != NULL) ? argv[i] : "";
    argv_ptrs[i] = push_string(&sp, arg);
  }

  uintptr_t envp_ptrs[MAX_STACK_ARGS];
  for (int i = 0; i < envc; i++) {
    envp_ptrs[i] = push_string(&sp, envp[i]);
  }

  // Reserve room for the pointer/word block below the strings:
  // argc + argv[argc] + NULL + envp[envc] + NULL + 3 auxv pairs
  size_t words = 1 + (size_t)argc + 1 + (size_t)envc + 1 + (3 * 2);
  uintptr_t block = sp - (words * sizeof(uintptr_t));
  // Round down so that the returned pointer is 16-byte aligned.
  block &= ~((uintptr_t)STACK_ALIGNMENT - 1);

  uintptr_t* p = (uintptr_t*)block;

  *p++ = (uintptr_t)argc;

  for (int i = 0; i < argc; i++) {
    *p++ = argv_ptrs[i];
  }
  *p++ = 0;

  for (int i = 0; i < envc; i++) {
    *p++ = envp_ptrs[i];
  }
  *p++ = 0;

  // Auxiliary vector.
  *p++ = AT_PAGESZ;
  *p++ = 4096;
  *p++ = AT_ENTRY;
  *p++ = entry;
  *p++ = AT_NULL;
  *p++ = 0;

  return block;
}
