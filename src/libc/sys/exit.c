#include <sys/syscall.h>

void exit(int code)
{
#ifdef __is_libk
  return k_exit(code);
#else
  syscall(SYSCALL_EXIT, code);
#endif
}
