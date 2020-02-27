#include "fd.h"

#define NB_FILE_DESCRIPTORS 10

file_descriptor_t descriptors[NB_FILE_DESCRIPTORS] = { 0 };

int create_file_descriptor(inode_t inode, uint32_t flags)
{
  // 0 (stdin), 1 (stdout) and 2 (stderr) are reserved.
  for (uint8_t fd = 3; fd < NB_FILE_DESCRIPTORS; fd++) {
    if (descriptors[fd].inode == 0) {
      descriptors[fd].inode = inode;
      descriptors[fd].offset = 0;
      descriptors[fd].flags = flags;

      return fd;
    }
  }

  return -1;
}

file_descriptor_t* get_file_descriptor(int fd)
{
  if (fd >= NB_FILE_DESCRIPTORS) {
    return 0;
  }

  return &descriptors[fd];
}

void delete_file_descriptor(int fd)
{
  if (fd >= NB_FILE_DESCRIPTORS) {
    return;
  }

  descriptors[fd].inode = 0;
  descriptors[fd].offset = 0;
  descriptors[fd].flags = 0;
}
