#include "process.h"
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
  elf_header_t* old_elf = NULL;

  if (current_process == NULL) {
    current_process = process_create_root();
  } else {
    // Give a new PID to the current process.
    current_process->pid++;
    // Save the old ELF pointer for later.
    old_elf = current_process->elf;
    // Free the current name as we'll update it right after.
    free(current_process->name);
    // Free old args.
    for (int i = 0; current_process->argv[i] != NULL; i++) {
      free(current_process->argv[i]);
    }
    free(current_process->argv);
    // Free old env vars.
    for (int i = 0; current_process->envp[i] != NULL; i++) {
      free(current_process->envp[i]);
    }
    free(current_process->envp);
  }

  // Set current process name.
  current_process->name = strdup(name);
  // Reset user stack.
  memset(current_process->user_stack, 0, USER_STACK_SIZE);

  // We need both `argc` and `argv` so we start by retrieving `argc`. Then, we
  // need to `strdup()` all the given arguments becaue `exec` replaces the
  // (old) process image so we won't have access to them for a very long time.
  int argc = 0;
  while (argv[argc]) {
    argc++;
  }

  char** _argv = (char**)malloc(sizeof(char*) * (argc + 1));
  for (int i = 0; i < argc; i++) {
    _argv[i] = strdup(argv[i]);
  }
  _argv[argc] = NULL;
  current_process->argv = _argv;

  // TODO: do not use a fixed size.
  char** envp = (char**)calloc(1, sizeof(char*) * 10);
  current_process->envp = envp;

  void* stack = (void*)&current_process->user_stack[USER_STACK_SIZE - 1];
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)envp);
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)_argv);
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)argc);
  current_process->user_rsp = (uint64_t)stack;

  if (old_elf) {
    // Unload current process.
    elf_unload(old_elf);
    free(old_elf);
  }

  // Load ELF in current process.
  elf_header_t* elf = elf_load(image);
  current_process->elf = elf;

  return current_process;
}
