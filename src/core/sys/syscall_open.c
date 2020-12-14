#include "logging.h"
#include "syscall.h"
#include <errno.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>

void syscall_open(registers_t* registers)
{
  errno = 0;

  const char* pathname = (const char*)registers->rbx;
  uint32_t flags = registers->rcx;

  inode_t inode = vfs_namei(pathname);

  if (inode == 0) {
    registers->rdx = -1;
    errno = ENOENT;
    return;
  }

  int fd = create_file_descriptor(inode, flags);

  if (fd == -1) {
    CORE_SYS_DEBUG("%s", "too many files open");
    registers->rdx = -1;
    errno = ENFILE;
    return;
  }

  registers->rdx = fd;

  CORE_SYS_DEBUG("open fd=%d inode=%p flags=%d", registers->rdx, inode, flags);
}
