#include <sys/syscall.h>

int reboot(int command)
{
  int ret;

  __asm__(INT_SYSCALL : "=d"(ret) : "a"(SYSCALL_REBOOT), "b"(command));

  return ret;
}
