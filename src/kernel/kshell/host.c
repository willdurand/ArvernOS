#include <kshell/kshell.h>

#include <arpa/inet.h>
#include <net/dns.h>
#include <stdio.h>
#include <sys/syscall.h>

void host(int argc, char* argv[])
{
  if (argc != 2) {
    printf("usage: %s <hostname>\n", argv[0]);
    return;
  }

  struct in_addr in;
  int retval = gethostbyname2(argv[1], &in);

  switch (retval) {
    case 0: {
      char buf[16];
      inet_ntoa2(in, buf, 16);
      printf("%s has address %s\n", argv[1], buf);
      break;
    }
    case DNS_ERR_NO_ANSWER:
      printf("Host %s not found\n", argv[1]);
      break;
    default:
      printf("DNS lookup failed (%d)\n", retval);
  }
}
