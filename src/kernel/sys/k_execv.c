#include <sys/k_syscall.h>

#include <arch/proc.h>
#include <core/elf.h>
#include <errno.h>
#include <fs/vfs.h>
#include <logging.h>
#include <proc/task.h>
#include <proc/user_stack.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/logging.h>

#ifdef __x86_64__
#include <mmu/paging.h>
#endif

// Size (in bytes) of the user stack. On architectures with a memory management
// unit, the whole stack has to fit in this range so that it can be mapped as
// user-accessible.
#define USER_STACK_SIZE (32 * 1024)

/// Maximum size of an executable we are willing to load.
#define EXEC_MAX_FILE_SIZE (16 * 1024 * 1024)
/// Maximum number of arguments.
#define EXEC_MAX_ARGS 64

static const char* path_basename(const char* path)
{
  const char* base = path;

  for (const char* p = path; *p != '\0'; p++) {
    if (*p == '/') {
      base = p + 1;
    }
  }

  return base;
}

int k_execv(const char* path, char* const argv[])
{
  task_t* task = CURRENT_TASK;

  if (task == NULL || path == NULL) {
    return -EINVAL;
  }

  // Copy the path into kernel memory: user memory will be overwritten once the
  // new program is loaded.
  char* kpath = strdup(path);
  if (kpath == NULL) {
    return -ENOMEM;
  }

  inode_t inode = vfs_namei(kpath);
  if (inode == NULL) {
    free(kpath);
    return -ENOENT;
  }

  vfs_stat_t st;
  vfs_stat(inode, &st);
  if (st.size <= 0 || st.size > EXEC_MAX_FILE_SIZE) {
    free(kpath);
    return -ENOEXEC;
  }

  uint8_t* buffer = malloc((size_t)st.size);
  if (buffer == NULL) {
    free(kpath);
    return -ENOMEM;
  }

  uint64_t read = vfs_read(inode, buffer, (uint64_t)st.size, 0);
  if (read != (uint64_t)st.size) {
    free(buffer);
    free(kpath);
    return -EINVAL;
  }

  // Copy `argv` into kernel memory before loading the new program (which may
  // reuse the caller's address space).
  int argc = 0;
  if (argv != NULL) {
    while (argc < EXEC_MAX_ARGS && argv[argc] != NULL) {
      argc++;
    }
  }

  char** kargv = malloc(sizeof(char*) * ((size_t)argc + 1));
  if (kargv == NULL) {
    free(buffer);
    free(kpath);
    return -ENOMEM;
  }

  for (int i = 0; i < argc; i++) {
    kargv[i] = strdup(argv[i]);
    if (kargv[i] == NULL) {
      for (int j = 0; j < i; j++) {
        free(kargv[j]);
      }
      free(kargv);
      free(buffer);
      free(kpath);
      return -ENOMEM;
    }
  }
  kargv[argc] = NULL;

#if defined(__x86_64__)
  // Load the new program into a fresh address space. This mirrors `fork()`:
  // the task no longer shares its user pages with anyone.
  void* old_space = task->address_space;
  void* new_space = paging_create_user_space();
  if (new_space == NULL) {
    for (int i = 0; i < argc; i++) {
      free(kargv[i]);
    }
    free(kargv);
    free(buffer);
    free(kpath);
    return -ENOMEM;
  }

  paging_activate(new_space);
  task->address_space = new_space;

  if (old_space != NULL) {
    paging_deactivate(old_space);
  }
#endif

  elf_header_t* elf = elf_load(buffer);
  uintptr_t entry = (elf != NULL) ? (uintptr_t)elf->entry : 0;
  free(buffer);

  if (elf == NULL) {
    for (int i = 0; i < argc; i++) {
      free(kargv[i]);
    }
    free(kargv);
    free(kpath);
    return -ENOEXEC;
  }

#if defined(__x86_64__)
  uintptr_t stack_top = USER_SPACE_END;
  uintptr_t stack_base = stack_top - USER_STACK_SIZE;

  map_multiple(page_containing_address(stack_base),
               USER_STACK_SIZE / PAGE_SIZE,
               PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE |
                 PAGING_FLAG_USER_ACCESSIBLE | PAGING_FLAG_NO_EXECUTE);
#else
  uintptr_t stack_base = (uintptr_t)malloc(USER_STACK_SIZE);
  if (stack_base == 0) {
    for (int i = 0; i < argc; i++) {
      free(kargv[i]);
    }
    free(kargv);
    free(kpath);
    return -ENOMEM;
  }
  task->user_stack_base = stack_base;
  uintptr_t stack_top = stack_base + USER_STACK_SIZE;
#endif

  uintptr_t user_sp = create_user_stack(stack_top, argc, kargv, NULL, entry);
  if (user_sp == 0) {
    for (int i = 0; i < argc; i++) {
      free(kargv[i]);
    }
    free(kargv);
    free(kpath);
    return -ENOMEM;
  }

  const char* base = path_basename(kpath);
  task_set_name(task, base);
  task->flags &= ~KERNEL_TASK;

  SYS_DEBUG("entering user mode: entry=%p argv[0]=%s user_sp=%p",
            (void*)entry,
            task->name,
            user_sp);

  for (int i = 0; i < argc; i++) {
    free(kargv[i]);
  }
  free(kargv);
  free(kpath);

  arch_task_set_kernel_stack(task);
  arch_task_enter_user(entry, user_sp, task);

  // `arch_task_enter_user()` never returns.
  __builtin_unreachable();
}
