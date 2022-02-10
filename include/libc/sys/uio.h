/** @file */
#ifndef SYS_UIO_H
#define SYS_UIO_H

struct iovec
{
  uintptr_t base;
  ssize_t len;
} __attribute__((packed));

#endif
