#include <sys/syscall.h>

pid_t getpid()
{
#ifdef __is_libk
  return k_getpid();
#else
  return syscall(SYSCALL_GETPID);
#endif
}
