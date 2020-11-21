#include <sys/syscall.h>

int reboot(int command)
{
  int ret = 0;

  __asm__(INT_SYSCALL : "=d"(ret) : "a"(SYSCALL_REBOOT), "b"(command));

  return ret;
}
