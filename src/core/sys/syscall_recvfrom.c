#include "logging.h"
#include "syscall.h"
#include <arpa/inet.h>
#include <core/utils.h>
#include <errno.h>
#include <net/socket.h>
#include <proc/descriptor.h>
#include <stddef.h>

void syscall_recvfrom(registers_t* registers)
{
  errno = 0;

  int sockfd = (int)registers->rsi;
  void* buf = (void*)registers->rdx;
  size_t len = (size_t)registers->rcx;
  int flags = (int)registers->r10;
  struct sockaddr* src_addr = (struct sockaddr*)registers->r8;
  socklen_t* addrlen = (socklen_t*)registers->r9;

  UNUSED(addrlen);
  UNUSED(src_addr);
  UNUSED(flags);

  if (sockfd < 3) {
    CORE_SYS_DEBUG("invalid socket descriptor sd=%d", sockfd);
    registers->rdx = -1;
    errno = ENOTSOCK;
    return;
  }

  descriptor_t* desc = get_descriptor(sockfd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("socket descriptor sockfd=%d not found", sockfd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  if (desc->inode != NULL) {
    CORE_SYS_DEBUG("sockfd=%d is not a socket descriptor", sockfd);
    registers->rdx = -1;
    errno = ENOTSOCK;
    return;
  }

  if (desc->domain != AF_INET || desc->type != SOCK_DGRAM ||
      !is_protocol_supported(desc->type, desc->protocol)) {
    CORE_SYS_DEBUG("invalid sockfd=%d", sockfd);
    registers->rdx = -1;
    errno = EINVAL;
    return;
  }

  registers->rdx = socket_unbufferize(sockfd, (uint8_t*)buf, len);
}
