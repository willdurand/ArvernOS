/**
 * @file
 *
 * Tar works by concatenating all files together in one big file where each
 * file gets a header that is padded up to 512 bytes. The headers are each
 * aligned on a 512 byte boundry and the content follows directly afterwards.
 */
#ifndef FS_TAR_H
#define FS_TAR_H

#include <fs/vfs.h>

#define TAR_FILE      '0'
#define TAR_DIRECTORY '5'

typedef struct tar_header
{
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char type;
} tar_header_t;

/**
 * @brief Creates a TAR-based read-only file system.
 *
 * This function creates a TAR-based read-only file system by reading an
 * archive stored in memory at the address specified as first argument. This
 * file system can have directories and files. The archive used is also called
 * a (init) "ram disk" because the GRUB will load the entire archive in memory.
 *
 * @param address the memory address where the TAR archive is located
 * @return the "root" inode of the tar file system
 */
inode_t tar_fs_init(uint64_t address);

#endif
