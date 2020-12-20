#include <sys/syscall.h>

int socket(int domain, int type, int protocol)
{
#ifdef __is_libk
  return k_socket(domain, type, protocol);
#else
  errno = 0;
  int retval;

  __asm__(INT_SYSCALL
          : "=d"(retval)
          : "a"(SYSCALL_SOCKET), "b"(domain), "c"(type), "S"(protocol));

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
