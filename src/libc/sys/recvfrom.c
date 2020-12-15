#include <sys/syscall.h>

ssize_t recvfrom(int sockfd,
                 void* buf,
                 size_t len,
                 int flags,
                 struct sockaddr* src_addr,
                 socklen_t* addrlen)
{
  ssize_t retval;

  register int r10 __asm__("r10") = flags;
  register struct sockaddr* r8 __asm__("r8") = src_addr;
  register socklen_t* r9 __asm__("r9") = addrlen;

  // We implement blocking here because the kernel does not run mulitple
  // processes/threads. It cannot be done in the syscall handler either because
  // it would block everything else.
  do {
#ifdef __is_libk
    retval = k_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
#else
    errno = 0;

    __asm__(INT_SYSCALL
            : "=d"(retval)
            : "a"(SYSCALL_RECVFROM),
              "S"(sockfd),
              "d"(buf),
              "c"(len),
              "r"(r10),
              "r"(r8),
              "r"(r9));

    SYSCALL_SET_ERRNO();
#endif
  } while (retval == 0);

  return retval;
}
