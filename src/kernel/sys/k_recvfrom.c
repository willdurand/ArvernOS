#include <sys/k_syscall.h>

#include <arpa/inet.h>
#include <arvern/utils.h>
#include <errno.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <string.h>
#include <sys/logging.h>

ssize_t k_recvfrom(int sockfd,
                   void* buf,
                   size_t len,
                   int flags,
                   struct sockaddr* src_addr,
                   socklen_t* addrlen)
{
  UNUSED(flags);

  descriptor_t* desc = get_descriptor(sockfd);

  if (desc == NULL) {
    SYS_DEBUG("socket descriptor sockfd=%d not found", sockfd);
    return -EBADF;
  }

  if (desc->domain != AF_INET) {
    SYS_DEBUG("invalid domain for sockfd=%d", sockfd);
    return -EINVAL;
  }

  switch (desc->type) {
    case SOCK_DGRAM:
    case SOCK_RAW:
      break;
    default:
      SYS_DEBUG("invalid type for sockfd=%d", sockfd);
      return -EINVAL;
  }

  if (!is_protocol_supported(desc->type, desc->protocol)) {
    SYS_DEBUG("unsupported protocol for sockfd=%d", sockfd);
    return -EINVAL;
  }

  memcpy(src_addr, &desc->addr, desc->addr_len);
  *addrlen = desc->addr_len;

  return vfs_read(desc->inode, buf, len, 0);
}
