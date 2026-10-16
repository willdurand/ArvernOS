#include <sys/k_syscall.h>

#include <arch/proc.h>
#include <arvern/utils.h>
#include <core/elf.h>
#include <errno.h>
#include <fs/vfs.h>
#include <proc/task.h>
#include <proc/user_stack.h>
#include <sys/logging.h>

/// Maximum size of an executable we are willing to load.
#define EXEC_MAX_FILE_SIZE (16 * 1024 * 1024)

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

  int argc = 0;
  if (argv != NULL) {
    while (argv[argc] != NULL) {
      argc++;
    }
  }

  uintptr_t s[USER_STACK_SIZE / sizeof(uintptr_t)];
  uintptr_t stack_top = (uintptr_t)&s[USER_STACK_SIZE / sizeof(uintptr_t)];
  uintptr_t user_sp = create_user_stack(stack_top, argc, argv, NULL, entry);

  if (user_sp == 0) {
    return -ENOMEM;
  }

  SYS_DEBUG("entering user mode: entry=%p argv[0]=%s user_sp=%p",
            (void*)entry,
            task->name,
            user_sp);

  arch_task_enter_user(entry, user_sp, task);

  // `arch_task_enter_user()` never returns.
  __builtin_unreachable();
}
