#include "descriptor.h"
#include <string.h>
#include <sys/socket.h>

#define NB_SYSTEM_DESCRIPTORS 20

static descriptor_t descriptors[NB_SYSTEM_DESCRIPTORS] = { 0 };

static uint16_t socket_base_port = 49152;

int create_file_descriptor(inode_t inode, uint32_t flags)
{
  // 0 (stdin), 1 (stdout) and 2 (stderr) are reserved.
  for (uint8_t fd = 3; fd < NB_SYSTEM_DESCRIPTORS; fd++) {
    if (!descriptors[fd].used) {
      descriptors[fd].used = true;
      descriptors[fd].inode = inode;
      descriptors[fd].offset = 0;
      descriptors[fd].flags = flags;
      descriptors[fd].domain = 0;
      descriptors[fd].type = 0;
      descriptors[fd].protocol = 0;
      descriptors[fd].port = 0;

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

int create_socket_descriptor(inode_t inode,
                             uint32_t domain,
                             uint32_t type,
                             uint32_t protocol)
{
  for (uint8_t fd = 3; fd < NB_SYSTEM_DESCRIPTORS; fd++) {
    if (!descriptors[fd].used) {
      descriptors[fd].used = true;
      descriptors[fd].inode = inode;
      descriptors[fd].offset = 0;
      descriptors[fd].flags = 0;
      descriptors[fd].domain = domain;
      descriptors[fd].type = type;
      descriptors[fd].protocol = protocol;
      descriptors[fd].port = socket_base_port + fd;

      return fd;
    }
  }

  return -1;
}

int descriptor_udp_lookup(uint16_t port)
{
  for (uint8_t fd = 3; fd < NB_SYSTEM_DESCRIPTORS; fd++) {
    if (descriptors[fd].used && descriptors[fd].port == port) {
      return fd;
    }
  }

  return -1;
}

bool is_protocol_supported(uint32_t type, uint32_t protocol)
{
  switch (type) {
    case SOCK_DGRAM:
      switch (protocol) {
        case IPPROTO_UDP:
          return true;
      }
      break;
  }

  return false;
}
