/**
 * @file
 * @see https://labs.apnic.net/?p=462
 * @see http://seriot.ch/ntp.php
 */
#ifndef NET_NTP_H
#define NET_NTP_H

#include <net/net.h>
#include <net/udp.h>
#include <sys/types.h>

#define NTP_MODE_CLIENT 3
#define NTP_V4          (4 << 3)

#define NTP_ERR_SOCK 1
#define NTP_ERR_SEND 2
#define NTP_ERR_RECV 3
#define NTP_ERR_HOST 4

#define NTP_TIMESTAMP_DELTA 2208988800ull

// Seconds and fractions since 01.01.1900
typedef struct ntp_timestamp
{
  uint32_t seconds;
  int32_t fraction;
} ntp_timestamp_t;

typedef struct ntp_header
{
  // Leap Indicator (2 bits)
  // NTP Version Number (3 bits)
  // NTP packet mode (3 bits)
  uint8_t li_vn_mode;
  uint8_t stratum;
  int8_t poll;
  int8_t precision;
  int32_t root_delay;
  int32_t root_dispersion;
  uint32_t reference_id;
  /// The time the system clock was last set or corrected.
  ntp_timestamp_t reference_ts;
  /// The time at which the request departed the client for the server.
  ntp_timestamp_t origin_ts;
  /// The time at which the client request arrived at the server.
  ntp_timestamp_t receive_ts;
  /// The time at the server when the response left for the client.
  ntp_timestamp_t transmit_ts;
} __attribute__((packed)) ntp_header_t;

int ntp_request(net_interface_t* interface,
                const char* server,
                time_t* server_time);

#endif
