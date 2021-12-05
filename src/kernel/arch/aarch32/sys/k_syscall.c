#include <sys/k_syscall.h>

#include <arvern/utils.h>

void syscall_init() {}

int k_execv(const char* path, char* const argv[])
{
  UNUSED(*path);
  UNUSED(*argv);

  return -1;
}

pid_t k_getpid()
{
  return -1;
}
