#ifndef FS_VFS_H
#define FS_VFS_H

#include <stdint.h>

#define VFS_NAME_MAX_SIZE 256

#define FS_MOUNTPOINT  0x01
#define FS_FILE        0x02
#define FS_DIRECTORY   0x04
#define FS_CHARDEVICE  0x08
#define FS_BLOCKDEVICE 0x10
#define FS_PIPE        0x20
#define FS_SYMLINK     0x40
#define FS_MASK        0xFE

#define FS_ERR_NOTAMOUNTPOINT 1
#define FS_ERR_HASCHILDREN    2
#define FS_ERR_NOTFOUND       3
#define FS_ERR_HASPARENT      4

struct vfs_node;

typedef struct vfs_node* inode_t;

typedef struct _stat
{
  uint64_t size;
} stat_t;

typedef struct dirent
{
  char name[VFS_NAME_MAX_SIZE];
  inode_t inode;
} dirent_t;

typedef struct vfs_driver
{
  uint64_t (*open)(inode_t inode, uint64_t mode);
  uint64_t (*close)(inode_t inode);
  uint64_t (*read)(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
  uint64_t (*write)(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
  uint64_t (*stat)(inode_t inode, stat_t* stat);
  uint64_t (*isatty)(inode_t inode);
  dirent_t* (*readdir)(inode_t inode, uint64_t num);
  inode_t (*finddir)(inode_t inode, const char* name);
  void (*cleanup)(inode_t inode);
} vfs_driver_t;

typedef struct vfs_node
{
  char name[VFS_NAME_MAX_SIZE];
  uint64_t type;
  vfs_driver_t* driver;
  inode_t parent;
  uint64_t n_children;
  inode_t* children;
  int data;
} vfs_node_t;

void vfs_init();

uint64_t vfs_open(inode_t inode, uint64_t mode);

uint64_t vfs_close(inode_t inode);

uint64_t vfs_read(inode_t inode, void* ptr, uint64_t length, uint64_t offset);

uint64_t vfs_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset);

uint64_t vfs_stat(inode_t inode, stat_t* stat);

uint64_t vfs_isatty(inode_t inode);

dirent_t* vfs_readdir(inode_t inode, uint64_t num);

inode_t vfs_finddir(inode_t inode, const char* name);

inode_t vfs_mount(const char* path, inode_t root);

inode_t vfs_namei(const char* path);

int vfs_umount(const char* path);

int vfs_free(inode_t inode);

int vfs_inode_type(inode_t inode);

void vfs_debug(inode_t node, int depth);

#endif
