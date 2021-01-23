#include "kshell.h"
#include <net/net.h>
#include <net/ntp.h>
#include <stdio.h>
#include <time.h>

void ntp(int argc, char* argv[])
{
  if (argc != 2) {
    printf("usage: %s <time server>\n", argv[0]);
    return;
  }

  time_t server_time;
  int retval = ntp_request(net_get_interface(0), argv[1], &server_time);

  switch (retval) {
    case 0: {
      char buf[40];
      strftime(buf, 40, "%c", localtime(&server_time));
      printf("server time is: %s\n", buf);
      break;
    }
    default:
      printf("Nope. (%d)\n", retval);
  }
}
