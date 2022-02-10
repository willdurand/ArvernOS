#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <errno.h>
#include <fcntl.h>
#include <proc/task.h>
#include <stdlib.h>
#include <string.h>
#include <sys/logging.h>
#include <sys/stat.h>

#ifdef CONFIG_LINUX_COMPAT

#include <core/elf.h>

#define PUSH_TO_STACK(stack, type, value)                                      \
  stack = (char*)stack - sizeof(type);                                         \
  *((type*)stack) = value

// A trampoline function that jumps to `fn` after having set `rsp` to the value
// of `stack`.
extern void linux_compat_start(uintptr_t fn, uintptr_t stack);

#endif // CONFIG_LINUX_COMPAT

int k_execv(const char* path, char* const argv[])
{
#ifdef CONFIG_LINUX_COMPAT
  int fd = k_open(path, O_RDONLY);
  if (fd < 3) {
    SYS_DEBUG("error k_open: path=%s fd=%d", path, fd);
    return fd;
  }

  struct stat stat = { 0 };

  int retval = k_fstat(fd, &stat);
  if (retval != 0) {
    SYS_DEBUG("%s", "error k_fstat");
    return retval;
  }

  uint8_t* buf = (uint8_t*)malloc(stat.st_size * sizeof(uint8_t));

  retval = k_read(fd, buf, stat.st_size);
  if (retval != stat.st_size) {
    SYS_DEBUG("%s", "error k_read");
    return retval;
  }

  retval = k_close(fd);
  if (retval != 0) {
    SYS_DEBUG("%s", "error k_close");
    return retval;
  }

  elf_header_t* elf = elf_load(buf);

  if (elf == NULL) {
    return -ENOEXEC;
  }

  strncpy(CURRENT_TASK->name, path, 20);

  uint64_t argc = 0;
  while (argv[argc]) {
    argc++;
  }

  uint64_t s[4096];
  void* stack = (void*)&s[4095];

  // aux: AT_NULL
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);
  // aux: AT_RANDOM
  // TODO: `AT_RANDOM` should really be random...
  uint8_t rand_bytes[16] = {
    0xaa, 0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee, 0xc0,
    0xff, 0xee, 0xc0, 0xff, 0xee, 0xc0, 0xff, 0xee,
  };
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)rand_bytes);
  PUSH_TO_STACK(stack, uint64_t, 25);
  // aux: AT_PAGESZ
  PUSH_TO_STACK(stack, uint64_t, 4096);
  PUSH_TO_STACK(stack, uint64_t, 6);
  // aux: AT_PHNUM
  PUSH_TO_STACK(stack, uint64_t, elf->ph_num);
  PUSH_TO_STACK(stack, uint64_t, 5);
  // aux: AT_PHENT
  PUSH_TO_STACK(stack, uint64_t, elf->ph_size);
  PUSH_TO_STACK(stack, uint64_t, 4);
  // aux: AT_PHDR
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)buf + elf->ph_offset);
  PUSH_TO_STACK(stack, uint64_t, 3);

  // envp
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);

  // argv
  PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)NULL);

  for (int i = argc - 1; i >= 0; i--) {
    PUSH_TO_STACK(stack, uintptr_t, (uintptr_t)argv[i]);
  }

  // argc
  PUSH_TO_STACK(stack, uint64_t, argc);

  linux_compat_start((uintptr_t)elf->entry, (uintptr_t)stack);

  elf_unload(elf);
  free(buf);

  return retval;
#else  // CONFIG_LINUX_COMPAT
  UNUSED(path);
  UNUSED(*argv);

  return -ENOSYS;
#endif // CONFIG_LINUX_COMPAT
}
