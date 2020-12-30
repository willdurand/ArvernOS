#include <sys/syscall.h>

ssize_t sendto(int sockfd,
               const void* buf,
               size_t len,
               int flags,
               const struct sockaddr* dst_addr,
               socklen_t addrlen)
{
#ifdef __is_libk
  return k_sendto(sockfd, buf, len, /* flags, */ dst_addr, addrlen);
#else
  errno = 0;

  ssize_t retval =
    syscall(SYSCALL_SENDTO, sockfd, buf, len, /* flags, */ dst_addr, addrlen);

  SYSCALL_SET_ERRNO();

  return retval;
#endif
}
