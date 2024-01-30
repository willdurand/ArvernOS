#include <sys/k_syscall.h>

#include <sys/logging.h>
#include <sys/uio.h>

int k_writev(int fd, struct iovec* iov, int iovcnt)
{
  SYS_DEBUG("fd=%d iovcnt=%d", fd, iovcnt);

  int retval = 0;
  for (int i = 0; i < iovcnt; i++) {
    int rv = k_write(fd, (void*)iov[i].base, iov[i].len);

    if (rv < 0) {
      return rv;
    }

    retval += rv;
  }

  return retval;
}
