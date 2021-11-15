#include <sys/k_syscall.h>

#include <errno.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/logging.h>

int k_close(int fd)
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

  int ret = 0;
  if (desc->inode != NULL) {
    ret += vfs_close(desc->inode);
  }

  delete_descriptor(fd);

  SYS_DEBUG("close fd=%d ret=%d", fd, ret);
  return ret;
}
