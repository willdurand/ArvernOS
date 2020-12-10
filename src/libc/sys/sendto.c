#include <sys/syscall.h>

ssize_t sendto(int sockfd,
               const void* buf,
               size_t len,
               int flags,
               const struct sockaddr* dst_addr,
               socklen_t addrlen)
{
  ssize_t ret;

  register int r10 __asm__("r10") = flags;
  register const struct sockaddr* r8 __asm__("r8") = dst_addr;
  register socklen_t r9 __asm__("r9") = addrlen;

  __asm__(INT_SYSCALL
          : "=d"(ret)
          : "a"(SYSCALL_SENDTO),
            "S"(sockfd),
            "d"(buf),
            "c"(len),
            "r"(r10),
            "r"(r8),
            "r"(r9));

  return ret;
}
