#include "descriptor.h"
#include <string.h>

#define NB_SYSTEM_DESCRIPTORS 10

static descriptor_t descriptors[NB_SYSTEM_DESCRIPTORS] = { 0 };

int create_file_descriptor(inode_t inode, uint32_t flags)
{
  // 0 (stdin), 1 (stdout) and 2 (stderr) are reserved.
  for (uint8_t fd = 3; fd < NB_SYSTEM_DESCRIPTORS; fd++) {
    if (!descriptors[fd].used) {
      descriptors[fd].used = true;
      descriptors[fd].inode = inode;
      descriptors[fd].offset = 0;
      descriptors[fd].flags = flags;

      return fd;
    }
  }

  return -1;
}

descriptor_t* get_descriptor(int id)
{
  if (id >= NB_SYSTEM_DESCRIPTORS) {
    return NULL;
  }

  return &descriptors[id];
}

void delete_descriptor(int id)
{
  if (id >= NB_SYSTEM_DESCRIPTORS) {
    return;
  }

  memset(&descriptors[id], 0, sizeof(descriptor_t));
}
