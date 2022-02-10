#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <errno.h>
#include <net/net.h>
#include <net/udp.h>
#include <proc/descriptor.h>
#include <string.h>
#include <sys/logging.h>

ssize_t k_sendto(int sockfd,
                 const void* buf,
                 size_t len,
                 int flags,
                 const struct sockaddr* dst_addr,
                 socklen_t addrlen)
{
  UNUSED(flags);

  SYS_DEBUG("sockfd=%d buf=%p len=%d flags=%d", sockfd, buf, len, flags);

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

  SYS_DEBUG("descriptor: domain=%d type=%d protocol=%d",
            desc->domain,
            desc->type,
            desc->protocol);

  memcpy(&desc->addr, dst_addr, addrlen);
  desc->addr_len = addrlen;

  net_interface_t* interface = net_get_interface(0);

  switch (desc->type) {
    case SOCK_DGRAM:
      switch (desc->protocol) {
        case IPPROTO_IP:
          ipv4_send_packet(interface,
                           (struct sockaddr_in*)&desc->addr,
                           IPV4_PROTO_ICMP,
                           0,
                           (uint8_t*)buf,
                           len);
          return len;

        case IPPROTO_UDP:
          udp_send_packet(interface,
                          desc->port,
                          interface->mac,
                          (struct sockaddr_in*)&desc->addr,
                          (uint8_t*)buf,
                          len);
          return len;

        default:
          // Indicate that something went wrong, even though the type and
          // protocol are supported.
          return 0;
      }

    case SOCK_RAW:
      switch (desc->protocol) {
        case IPPROTO_ICMP:
          ipv4_send_packet(interface,
                           (struct sockaddr_in*)&desc->addr,
                           IPV4_PROTO_ICMP,
                           0,
                           (uint8_t*)buf,
                           len);
          return len;

        default:
          // Indicate that something went wrong, even though the type and
          // protocol are supported.
          return 0;
      }
  }

  return 0;
}
