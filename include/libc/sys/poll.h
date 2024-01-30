/** @file */
#ifndef SYS_POLL_H
#define SYS_POLL_H

struct pollfd
{
  int fd;
  short events;
  short revents;
};

typedef unsigned int nfds_t;

#endif
