#include "logging.h"
#include "syscall.h"
#include <errno.h>
#include <fs/vfs.h>
#include <net/socket.h>
#include <proc/descriptor.h>
#include <stddef.h>

void syscall_close(registers_t* registers)
{
  errno = 0;

  int fd = (int)registers->rbx;

  if (fd < 3) {
    CORE_SYS_DEBUG("invalid file descriptor fd=%d", fd);
    registers->rdx = -1;
    errno = EPERM;
    return;
  }

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("file descriptor fd=%d not found", fd);
    registers->rdx = -1;
    errno = EBADF;
    return;
  }

  if (desc->inode != NULL) {
    registers->rdx = vfs_close(desc->inode);
  }

  if (desc->port != 0) {
    registers->rdx = socket_delete_buffer(fd);
  }

  delete_descriptor(fd);

  CORE_SYS_DEBUG("close fd=%d", fd);
}
