#include <sys/syscall.h>

int socket(int domain, int type, int protocol)
{
#ifdef __is_libk
  return k_socket(domain, type, protocol);
#else
  int retval = syscall(SYSCALL_SOCKET, domain, type, protocol);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
