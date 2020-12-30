#include <sys/syscall.h>

int execv(const char* path, char* const argv[])
{
#ifdef __is_libk
  return k_execv(path, argv);
#else
  return syscall(SYSCALL_EXECV, path, argv);
#endif
}
