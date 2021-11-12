#include "process.h"
#include "logging.h"
#include <mmu/paging.h>
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

process_t* process_exec(uint8_t* image,
                        const char* name,
                        char* const argv[],
                        char* const envp[])
{
  PROC_DEBUG("image=%p name=%s", image, name);

  elf_header_t* old_elf = NULL;

  // The top of the stack is right below 0x40000000, which is where we load
  // userland programs.
  void* stack = (void*)0x40000000;

  if (current_process == NULL) {
    current_process = process_create_root();

    // Map the user stack with the user accessible flag set.
    map_multiple(page_containing_address((uint64_t)stack - (4 * PAGE_SIZE)),
                 4,
                 PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE |
                   PAGING_FLAG_USER_ACCESSIBLE);

  } else {
    // Save the old ELF pointer for later.
    old_elf = current_process->elf;
    // Free the current name as we'll update it right after.
    free(current_process->name);
    // Clear previous stack.
    memset((void*)current_process->user_rsp,
           0,
           0x40000000 - current_process->user_rsp);
  }

  // Set current process name.
  current_process->name = strdup(name);

  // We need both `argc` and `argv` so we start by retrieving `argc`.
  uint8_t argc = 0;
  while (argv[argc]) {
    argc++;
  }

  // In order to pass argv/envp to the userland, we need to prepare the user
  // stack. We'll start by pushing the different values in argv.
  char* _argv_ptrs[argc];
  for (int i = 0; i < argc; i++) {
    PUSHSTR_TO_STACK(stack, argv[i]);
    _argv_ptrs[i] = (char*)stack;
    free(argv[i]);
  }
  free((void*)argv);

  uint8_t envc = 0;
  while (envp[envc]) {
    envc++;
  }

  // Now we can push the different values in envp.
  char* _envp_ptrs[envc];
  for (int i = 0; i < envc; i++) {
    PUSHSTR_TO_STACK(stack, envp[i]);
    _envp_ptrs[i] = (char*)stack;
    free(envp[i]);
  }
  free((void*)envp);

  PUSH_TO_STACK(stack, uintptr_t, 0); // envp[envc] = NULL
  for (int i = envc - 1; i >= 0; i--) {
    PUSH_TO_STACK(stack, char*, _envp_ptrs[i]);
  }
  char** _envp = (char**)stack;

  PUSH_TO_STACK(stack, uintptr_t, 0); // argv[argc] = NULL
  for (int i = argc - 1; i >= 0; i--) {
    PUSH_TO_STACK(stack, char*, _argv_ptrs[i]);
  }
  char** _argv = (char**)stack;

  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)_envp);
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)_argv);
  PUSH_TO_STACK(stack, uint64_t, argc);
  current_process->user_rsp = (uintptr_t)stack;

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
