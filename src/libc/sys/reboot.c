#include <sys/syscall.h>

int reboot(int command)
{
#ifdef __is_libk
  return k_reboot(command);
#else
  int retval = syscall(SYSCALL_REBOOT, command);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
