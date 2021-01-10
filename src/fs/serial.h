/** @file */
#ifndef FS_SERIAL_H
#define FS_SERIAL_H

#include <fs/vfs.h>
#include <stdbool.h>
#include <sys/types.h>

#define FS_SERIAL_MOUNTPOINT_PATH "/dev/"
#define FS_SERIAL_COM1            "com1"

/**
 * Initializes the "serial" filesystem.
 *
 * This FS is a bit different than the other existing ones as it creates a set
 * of serial character devices instead of managing a single file or directory.
 */
bool serial_fs_init();

/**
 * Creates a "serial" character device.
 *
 * @return the inode of the serial character device for the given port
 */
inode_t serial_device_create(uint16_t serial_port);

#endif
