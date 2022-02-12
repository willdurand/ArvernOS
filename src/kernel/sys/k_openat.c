#include <sys/k_syscall.h>

#include <errno.h>
#include <fcntl.h>

int k_openat(int dirfd, const char* pathname, int flags)
{
  if (dirfd == AT_FDCWD) {
    return k_open(pathname, flags);
  }

  // TODO: Add better support for `dirfd` and other AT_* special values.

  return -EBADF;
}
