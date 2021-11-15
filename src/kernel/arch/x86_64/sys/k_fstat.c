#include <sys/k_syscall.h>

#include <errno.h>
#include <fs/vfs.h>
#include <inttypes.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/logging.h>
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

  vfs_stat_t stat = { 0 };
  vfs_stat(desc->inode, &stat);
  statbuf->st_size = stat.size;
  statbuf->st_mode = stat.mode;

  SYS_DEBUG("fd=%d st_size=%" PRIi64, fd, statbuf->st_size);

  return 0;
}
