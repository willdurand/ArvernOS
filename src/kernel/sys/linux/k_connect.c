#include <sys/k_syscall.h>

#include <proc/descriptor.h>
#include <string.h>

int k_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
  descriptor_t* desc = get_descriptor(sockfd);

  memcpy(&desc->addr, addr, addrlen);
  desc->addr_len = addrlen;

  return 0;
}
