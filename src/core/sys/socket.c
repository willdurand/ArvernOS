#include "syscall.h"
#include <core/debug.h>
#include <errno.h>
#include <net/socket.h>
#include <proc/descriptor.h>
#include <sys/socket.h>

void syscall_socket(registers_t* registers)
{
  errno = 0;

  int domain = (int)registers->rbx;
  int type = (int)registers->rcx;
  int protocol = (int)registers->rsi;

  if (domain != AF_INET) {
    registers->rdx = -1;
    errno = EAFNOSUPPORT;
    return;
  }

  if (type != SOCK_DGRAM) {
    registers->rdx = -1;
    errno = ESOCKTNOSUPPORT;
    return;
  }

  if (!is_protocol_supported(type, protocol)) {
    registers->rdx = -1;
    errno = EPROTONOSUPPORT;
    return;
  }

  int sd = create_socket_descriptor(domain, type, protocol);

  if (sd == -1) {
    DEBUG("%s", "too many files open");
    registers->rdx = -1;
    errno = ENFILE;
    return;
  }

  registers->rdx = sd;

  DEBUG("open sd=%d", registers->rdx);
}
