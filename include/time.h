#ifndef TIME_H
#define TIME_H

#include <stddef.h>
#include <sys/time.h>
#include <sys/types.h>

struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

time_t time(time_t* second);

struct tm* localtime(const time_t* time_ptr);

size_t strftime(char* str,
                size_t maxsize,
                const char* format,
                const struct tm* timeptr);

#endif
