#include <sys/syscall.h>

int execv(const char* path, char* const argv[])
{
#ifdef __is_libk
  return k_execv(path, argv);
#else
  int retval = syscall(SYSCALL_EXECV, path, argv);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
