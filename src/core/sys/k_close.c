#include "k_syscall.h"
#include "logging.h"
#include <errno.h>
#include <fs/vfs.h>
#include <net/socket.h>
#include <proc/descriptor.h>
#include <stddef.h>

int k_close(int fd)
{
  if (fd < 3) {
    CORE_SYS_DEBUG("invalid file descriptor fd=%d", fd);
    return -EPERM;
  }

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("file descriptor fd=%d not found", fd);
    return -EBADF;
  }

  int ret = 0;
  if (desc->inode != NULL) {
    ret = vfs_close(desc->inode);
  }

  if (desc->port != 0) {
    ret = socket_delete_buffer(fd);
  }

  delete_descriptor(fd);

  CORE_SYS_DEBUG("close fd=%d", fd);
  return ret;
}
