#ifndef SYS_TIME_H
#define SYS_TIME_H

#include <sys/types.h>

struct timeval
{
  time_t tv_sec;
  suseconds_t tv_usec;
};

struct timezone
{
  int tz_minuteswest;
  int tz_dsttime;
};

#endif
