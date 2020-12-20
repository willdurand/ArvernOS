#include "k_syscall.h"
#include "logging.h"
#include <errno.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>

int k_open(const char* pathname, uint32_t flags)
{
  // TODO: check flags

  inode_t inode = vfs_namei(pathname);

  if (inode == 0) {
    return -ENOENT;
  }

  int fd = create_file_descriptor(inode, flags);

  if (fd == -1) {
    SYS_DEBUG("%s", "too many files open");
    return -ENFILE;
  }

  SYS_DEBUG("open fd=%d inode=%p flags=%d", fd, inode, flags);

  return fd;
}
