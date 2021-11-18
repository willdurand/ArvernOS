#include <net/ntp.h>

#include <arpa/inet.h>
#include <arvern/utils.h>
#include <net/logging.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <time.h>

int ntp_request(net_interface_t* interface,
                const char* server,
                time_t* server_time)
{
  UNUSED(*interface);

  ntp_header_t ntp_header = {
    .li_vn_mode = NTP_V4 | NTP_MODE_CLIENT,
  };
  ntp_header.origin_ts.seconds = htonl(NTP_TIMESTAMP_DELTA + time(NULL));

  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sockfd < 0) {
    NET_DEBUG("failed to create a socket (sockfd=%d)", sockfd);
    return NTP_ERR_SOCK;
  }

  struct in_addr sin_addr;
  int retval = gethostbyname2(server, &sin_addr);

  if (retval != 0) {
    NET_DEBUG("failed to get host by name (retval=%d)", retval);
    return NTP_ERR_HOST;
  }

  struct sockaddr_in server_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT_NTP),
    .sin_addr = sin_addr,
  };

  socklen_t server_addr_len = sizeof(struct sockaddr_in);

  NET_DEBUG("%s", "sending ntp packet");

  if (sendto(sockfd,
             &ntp_header,
             sizeof(ntp_header_t),
             0,
             (struct sockaddr*)&server_addr,
             server_addr_len) < 0) {
    return NTP_ERR_SEND;
  }

  ssize_t bytes_received = recvfrom(sockfd,
                                    &ntp_header,
                                    sizeof(ntp_header_t),
                                    0,
                                    (struct sockaddr*)&server_addr,
                                    &server_addr_len);

  close(sockfd);

  if (bytes_received < (ssize_t)sizeof(ntp_header_t)) {
    NET_DEBUG("bytes_received=%lld", bytes_received);
    return NTP_ERR_RECV;
  }

  ntp_header.transmit_ts.seconds = ntohl(ntp_header.transmit_ts.seconds);
  ntp_header.transmit_ts.fraction =
    ntohl((uint32_t)ntp_header.transmit_ts.fraction);

  *server_time = ntp_header.transmit_ts.seconds - NTP_TIMESTAMP_DELTA;

  return 0;
}
