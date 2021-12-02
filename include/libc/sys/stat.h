/** @file */
#ifndef SYS_STATS_H
#define SYS_STATS_H

#include <sys/types.h>

#define S_IFCHR  0020000
#define S_IFDIR  0040000
#define S_IFREG  0100000
#define S_IFSOCK 0140000

struct stat
{
  off_t st_size;
  mode_t st_mode;
};

#endif
