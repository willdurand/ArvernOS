#include <sys/syscall.h>

int reboot(int command)
{
#ifdef __is_libk
  return k_reboot(command);
#else
  errno = 0;
  int retval;

  __asm__(INT_SYSCALL : "=d"(retval) : "a"(SYSCALL_REBOOT), "b"(command));

  SYSCALL_SET_ERRNO(retval);

  return retval;
#endif
}
