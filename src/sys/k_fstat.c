#include "k_syscall.h"
#include "logging.h"
#include <errno.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/types.h>

int k_fstat(int fd, struct stat* statbuf)
{
  if (fd < 3) {
    SYS_DEBUG("invalid file descriptor fd=%d", fd);
    return -EPERM;
  }

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    SYS_DEBUG("file descriptor fd=%d not found", fd);
    return -EBADF;
  }

  stat_t stat;
  vfs_stat(desc->inode, &stat);
  statbuf->st_size = stat.size;

  return 0;
}
