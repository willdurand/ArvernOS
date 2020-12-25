#include <sys/syscall.h>

int gethostbyname2(const char* name, struct in_addr* in)
{
#ifdef __is_libk
  return k_gethostbyname2(name, in);
#else
  return syscall(SYSCALL_GETHOSTBYNAME2, name, in);
#endif
}
