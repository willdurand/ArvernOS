#ifndef ERRNO_H
#define ERRNO_H

extern int errno;

#define EPERM           1
#define ENOENT          2
#define EBADF           9
#define ENOMEM          12
#define EINVAL          22
#define ENFILE          23
#define EMFILE          24
#define ENOSYS          38
#define ENOTSOCK        88
#define EPROTONOSUPPORT 93
#define ESOCKTNOSUPPORT 94
#define EAFNOSUPPORT    97

// Returns a pointer to a string that describes the error code passed in the
// argument `errnum`.
char* strerror(int errnum);

#endif
