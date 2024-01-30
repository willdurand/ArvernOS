#include <sys/k_syscall.h>

#include <proc/descriptor.h>
#include <sys/logging.h>

int k_dup2(int oldfd, int newfd)
{
  SYS_DEBUG("oldfd=%d newfd=%d", oldfd, newfd);

  duplicate_descriptor(oldfd, newfd);

  return newfd;
}
