#include <sys/syscall.h>

ssize_t recvfrom(int sockfd,
                 void* buf,
                 size_t len,
                 int flags,
                 struct sockaddr* src_addr,
                 socklen_t* addrlen)
{
  ssize_t retval;

  // We implement blocking here because the kernel does not run multiple
  // processes/threads. It cannot be done in the syscall handler either because
  // it would block everything else.
  do {
#ifdef __is_libk
    retval = k_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
#else
    errno = 0;

    retval =
      syscall(SYSCALL_RECVFROM, sockfd, buf, len, flags, src_addr, addrlen);

    SYSCALL_SET_ERRNO();
#endif
  } while (retval == 0);

  return retval;
}
