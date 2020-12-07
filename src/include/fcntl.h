/** @file */
#ifndef FCNTL_H
#define FCNTL_H

/// Open for reading only.
#define O_RDONLY 0x0000
/// Open for writing only.
#define O_WRONLY 0x0001
/// Open for reading and writing.
#define O_RDWR 0x0002
/// Starts writing at the end of the file.
#define O_APPEND 0x0008
/// Create then open the file.
#define O_CREAT 0x0100
/// Truncate file.
#define O_TRUNC 0x0200

/// Set file descriptor offset to offset.
#define SEEK_SET 0
/// Set file descriptor offset to current plus offset.
#define SEEK_CUR 1
/// Set file descriptor offset to EOF plus offset.
#define SEEK_END 2

#endif
