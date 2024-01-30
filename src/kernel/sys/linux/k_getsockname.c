#include <sys/k_syscall.h>

#include <proc/descriptor.h>
#include <string.h>

int k_getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
  descriptor_t* desc = get_descriptor(sockfd);

  memcpy(addr, &desc->addr, desc->addr_len);
  *addrlen = desc->addr_len;

  return 0;
}
