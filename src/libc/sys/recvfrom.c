#include <sys/syscall.h>

ssize_t recvfrom(int sockfd,
                 void* buf,
                 size_t len,
                 int flags,
                 struct sockaddr* src_addr,
                 socklen_t* addrlen)
{
  register int r10 __asm__("r10") = flags;
  register struct sockaddr* r8 __asm__("r8") = src_addr;
  register socklen_t* r9 __asm__("r9") = addrlen;

  ssize_t ret = -1;
  // We implement blocking here because the kernel does not run mulitple
  // processes/threads. It cannot be done in the syscall handler either because
  // it would block everything else.
  do {
    __asm__(INT_SYSCALL
            : "=d"(ret)
            : "a"(SYSCALL_RECVFROM),
              "S"(sockfd),
              "d"(buf),
              "c"(len),
              "r"(r10),
              "r"(r8),
              "r"(r9));
  } while (ret < 0);

  return ret;
}
