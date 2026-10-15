#include <sys/k_syscall.h>

#include <arch/proc.h>
#include <arvern/utils.h>
#include <core/elf.h>
#include <errno.h>
#include <fs/vfs.h>
#include <proc/task.h>
#include <sys/logging.h>

/// Maximum size of an executable we are willing to load.
#define EXEC_MAX_FILE_SIZE (16 * 1024 * 1024)

#define PUSH_TO_STACK(stack, type, value)                                      \
  stack = (char*)stack - sizeof(type);                                         \
  *((type*)stack) = value

int k_execv(const char* path, char* const argv[])
{
  task_t* task = CURRENT_TASK;

  if (task == NULL || path == NULL) {
    return -EINVAL;
  }

  // TODO: handle paths relative to the task's current working dir, which we
  // don't have either yet...
  inode_t inode = vfs_namei(path);
  if (inode == NULL) {
    return -ENOENT;
  }

  vfs_stat_t st;
  vfs_stat(inode, &st);
  if (st.size <= 0 || st.size > EXEC_MAX_FILE_SIZE) {
    return -ENOEXEC;
  }

  uint8_t* buffer = malloc((size_t)st.size);
  if (buffer == NULL) {
    return -ENOMEM;
  }

  uint64_t read = vfs_read(inode, buffer, (uint64_t)st.size, 0);
  if (read != (uint64_t)st.size) {
    free(buffer);
    return -EINVAL;
  }

  // TODO: validate ELF

  elf_header_t* elf = elf_load(buffer);
  uintptr_t entry = (elf != NULL) ? (uintptr_t)elf->entry : 0;
  free(buffer);

  if (elf == NULL) {
    return -ENOEXEC;
  }

  uint64_t s[4096];
  void* stack = (void*)&s[4095];

  // The initial stack is made of `uintptr_t`-sized words on all architectures.
  // That's what the crt0 of the different archs expect, and we need to support
  // 32-bit archs.

  // aux: AT_NULL
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);
  PUSH_TO_STACK(stack, uintptr_t, 0);
  // aux: AT_ENTRY
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)entry);
  PUSH_TO_STACK(stack, uintptr_t, 9);
  // aux: AT_PAGESZ
  PUSH_TO_STACK(stack, uintptr_t, 4096);
  PUSH_TO_STACK(stack, uintptr_t, 6);
  // envp
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);
  // argv
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)argv[0]);
  // argc
  PUSH_TO_STACK(stack, uintptr_t, 1);

  SYS_DEBUG(
    "entering user mode: entry=%p argv[0]=%s", (void*)entry, task->name);

  arch_task_enter_user(entry, (uintptr_t)stack, task);

  // `arch_task_enter_user()` never returns.
  __builtin_unreachable();
}
