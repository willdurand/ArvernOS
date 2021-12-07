#include <sys/k_syscall.h>

#include <arvern/utils.h>

int k_execv(const char* path, char* const argv[])
{
  UNUSED(*path);
  UNUSED(*argv);

  return k_not_implemented();
}
