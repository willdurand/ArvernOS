#include "syscall.h"
#include <core/debug.h>
#include <core/utils.h>
#include <errno.h>
#include <net/net.h>
#include <net/socket.h>
#include <net/udp.h>
#include <proc/descriptor.h>
#include <string.h>

void syscall_sendto(registers_t* registers)
{
  errno = 0;

  int sockfd = (int)registers->rsi;
  const void* buf = (const void*)registers->rdx;
  size_t len = (size_t)registers->rcx;
  int flags = (int)registers->r10;
  const struct sockaddr* dst_addr = (const struct sockaddr*)registers->r8;
  socklen_t addrlen = (socklen_t)registers->r9;

  UNUSED(flags);

  if (sockfd < 3) {
    DEBUG("invalid socket descriptor sd=%d", sockfd);
    registers->rdx = -1;
    errno = ENOTSOCK;
    return;
  }

  descriptor_t* desc = get_descriptor(sockfd);

  if (desc == NULL) {
    DEBUG("socket descriptor sockfd=%d not found", sockfd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  if (desc->inode != NULL) {
    DEBUG("sockfd=%d is not a socket descriptor", sockfd);
    registers->rdx = -1;
    errno = ENOTSOCK;
    return;
  }

  if (desc->domain != AF_INET || desc->type != SOCK_DGRAM ||
      !is_protocol_supported(desc->type, desc->protocol)) {
    DEBUG("invalid sockfd=%d", sockfd);
    registers->rdx = -1;
    errno = EINVAL;
    return;
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

  registers->rdx = len;
}
