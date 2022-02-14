#include "process.h"

#include <proc/logging.h>
#include <proc/usermode.h>
#include <stdlib.h>
#include <string.h>

static process_t* current_process = NULL;

process_t* process_create_root()
{
  current_process = (process_t*)malloc(sizeof(process_t));
  current_process->pid = 0;

  return current_process;
}

process_t* process_get_current()
{
  return current_process;
}

process_t* process_exec(uint8_t* image, const char* name, char* const argv[])
{
  PROC_DEBUG("image=%p name=%s", image, name);

  if (current_process == NULL) {
    current_process = process_create_root();
  } else {
    elf_unload(current_process->elf);
    free(current_process->elf);
    // Free the current name as we'll update it right after.
    free(current_process->name);
    // Give a new PID to the current process.
    current_process->pid++;
  }

  // Load ELF in current process.
  elf_header_t* elf = elf_load(image);
  current_process->elf = elf;

  // Set current process name.
  current_process->name = strdup(name);
  // Set up user stack.
  memset(current_process->user_stack, 0, USER_STACK_SIZE);

  // User stack:
  //
  //         +--------+ user_stack[0]
  //         |        |
  //         |        |
  //   ^     |        |
  //   |     |        |
  //  sp --->|        | user_stack[USER_STACK_TOP]
  //         |        |
  //         |        |
  //         |        |
  //         |        |
  // buf --->|        | user_stack[USER_STACK_BUF]
  //   |     |        |
  //   v     |        |
  //         |        |
  //         |        |
  //         +--------+ user_stack[USER_STACK_SIZE]
  //
  void* stack = (void*)&current_process->user_stack[USER_STACK_TOP];
  char* buf = (char*)&current_process->user_stack[USER_STACK_BUF];

  // auxv
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);

  // envp
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);

  // We need both `argc` and `argv` so we start by retrieving `argc`.
  uint64_t argc = 0;
  while (argv[argc]) {
    argc++;
  }

  // argv
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);

  size_t off = 0;
  for (int i = argc - 1; i >= 0; i--) {
    size_t len = strlen(argv[i]);
    // Copy argv[i] to high address in initial process stack.
    strncpy(&buf[off], argv[i], len + 1);

    // TODO: Check that we have enough space to write argv.

    // Add address of argv[i] to the stack.
    PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)&buf[off]);

    off += len + 1;
  }

  // argc
  PUSH_TO_STACK(stack, uint64_t, argc);

  current_process->user_rsp = (uintptr_t)stack;

  return current_process;
}
