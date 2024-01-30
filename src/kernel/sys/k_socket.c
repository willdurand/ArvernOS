#include <sys/k_syscall.h>

#include <errno.h>
#include <fcntl.h>
#include <fs/sock.h>
#include <proc/descriptor.h>
#include <sys/logging.h>

int k_socket(int domain, int type, int protocol)
{
  SYS_DEBUG("domain=%d type=%d protocol=%d", domain, type, protocol);

  if (domain != AF_INET) {
    SYS_DEBUG("domain %d not supported", domain);
    return -EAFNOSUPPORT;
  }

  switch (type) {
    case SOCK_DGRAM:
    case SOCK_RAW:
      break;
    default:
      SYS_DEBUG("type %d not supported", type);
      return -ESOCKTNOSUPPORT;
  }

  if (!is_protocol_supported(type, protocol)) {
    SYS_DEBUG("protocol %d not supported", protocol);
    return -EPROTONOSUPPORT;
  }

  inode_t parent = vfs_namei(FS_SOCK_MOUNTPOINT);

  if (parent == NULL) {
    SYS_DEBUG("%s", "parent not found");
    return -ENOENT;
  }

  // By passing `NULL`, the (sock) FS should generate a new name for the inode
  // before returning it.
  inode_t inode = vfs_create(parent, NULL, 0);

  if (inode == NULL) {
    SYS_DEBUG("%s", "failed to create new inode");
    return -EMFILE;
  }

  int sd = create_socket_descriptor(inode, domain, type, protocol);

  if (sd == -1) {
    SYS_DEBUG("%s", "too many files open");
    return -ENFILE;
  }

  SYS_DEBUG("open sd=%d inode=%p domain=%d type=%d protocol=%d",
            sd,
            inode,
            domain,
            type,
            protocol);

  return sd;
}
