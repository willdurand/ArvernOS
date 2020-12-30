#include "k_syscall.h"
#include "logging.h"
#include <core/elf.h>
#include <core/isr.h>
#include <fcntl.h>
#include <proc/process.h>
#include <proc/usermode.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static process_t* current_process = NULL;

int k_execv(const char* path, char* const argv[])
{
  int fd = k_open(path, O_RDONLY);
  if (fd < 3) {
    return fd;
  }

  struct stat stat = { 0 };

  int retval = k_fstat(fd, &stat);
  if (retval != 0) {
    return retval;
  }

  uint8_t* buf = (uint8_t*)malloc(stat.st_size * sizeof(uint8_t));

  retval = k_read(fd, buf, stat.st_size);
  if (retval != stat.st_size) {
    return retval;
  }

  retval = k_close(fd);
  if (retval != 0) {
    return retval;
  }

  elf_header_t* old_elf = NULL;

  if (current_process == NULL) {
    // Create the root process.
    current_process = (process_t*)malloc(sizeof(process_t));
    current_process->pid = 0;
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
  }

  // Set current process name.
  current_process->name = strdup(path);

  // Reset user stack.
  memset(current_process->user_stack, 0, sizeof(current_process->user_stack));

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

  void* stack = (void*)&current_process->user_stack[1023];
  // TODO: add support for `envp`
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)_argv);
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)argc);

  if (old_elf) {
    // Unload current process.
    elf_unload(old_elf);
    free(old_elf);
  }

  // Load ELF in current process.
  elf_header_t* elf = elf_load((uint8_t*)buf);
  current_process->elf = elf;

  SYS_DEBUG(
    "switching to process '%s': entrypoint=%p stack=%p argc=%d _argv=%p",
    current_process->name,
    current_process->elf->entry,
    stack,
    argc,
    _argv);

  switch_to_usermode((void*)current_process->elf->entry, stack);
}
