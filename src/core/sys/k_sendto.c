#include "logging.h"
#include "syscall.h"
#include <core/utils.h>
#include <errno.h>
#include <net/net.h>
#include <net/udp.h>
#include <proc/descriptor.h>
#include <string.h>

ssize_t k_sendto(int sockfd,
                 const void* buf,
                 size_t len,
                 int flags,
                 const struct sockaddr* dst_addr,
                 socklen_t addrlen)
{
  UNUSED(flags);

  if (sockfd < 3) {
    CORE_SYS_DEBUG("invalid socket descriptor sd=%d", sockfd);
    return -ENOTSOCK;
  }

  descriptor_t* desc = get_descriptor(sockfd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("socket descriptor sockfd=%d not found", sockfd);
    return -EBADF;
  }

  if (desc->inode != NULL) {
    CORE_SYS_DEBUG("sockfd=%d is not a socket descriptor", sockfd);
    return -ENOTSOCK;
  }

  if (desc->domain != AF_INET || desc->type != SOCK_DGRAM ||
      !is_protocol_supported(desc->type, desc->protocol)) {
    CORE_SYS_DEBUG("invalid sockfd=%d", sockfd);
    return -EINVAL;
  }

  net_interface_t* interface = net_get_interface(0);

  struct sockaddr_in addr = { 0 };
  memcpy(&addr, dst_addr, addrlen);

  switch (desc->protocol) {
    case IPPROTO_UDP:
      udp_send_packet(
        interface, desc->port, interface->mac, &addr, (uint8_t*)buf, len);
      break;
    default:
      // Indicate that something went wrong, even though the type and protocol
      // are supported.
      len = 0;
  }

  return len;
}
