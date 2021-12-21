#include <sys/k_syscall.h>

#include <errno.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/logging.h>

int k_open(const char* pathname, uint32_t flags)
{
  // TODO: Check flags.

  inode_t inode = vfs_namei(pathname);

  if (inode == 0) {
    SYS_DEBUG("%s not found", pathname);
    return -ENOENT;
  }

  int fd = create_file_descriptor(inode, flags);

  if (fd == -1) {
    SYS_DEBUG("%s", "too many files open");
    return -ENFILE;
  }

  SYS_DEBUG("fd=%d inode=%p flags=%d pathname=%s", fd, inode, flags, pathname);

  return fd;
}
