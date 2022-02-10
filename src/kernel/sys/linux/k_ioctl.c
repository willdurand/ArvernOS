#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <sys/logging.h>

int k_ioctl(int fd, int request, void* ptr)
{
  MAYBE_UNUSED(fd);
  MAYBE_UNUSED(request);
  MAYBE_UNUSED(ptr);

  SYS_DEBUG("fd=%d request=%d ptr=%p", fd, request, ptr);

  return 0;
}
