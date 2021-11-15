/** @file */
#ifndef WILLOS_H
#define WILLOS_H

#define KERNEL_ASCII                                                           \
  "               _  __\n"                                                     \
  "      o  |  | / \\(_ \n"                                                    \
  "  \\^/ |  |  | \\_/__)\n"

#ifndef KERNEL_NAME
#define KERNEL_NAME "noname OS"
#endif

#ifdef ENABLE_KERNEL_DEBUG
#define KERNEL_VERSION "DEBUG MODE"
#else
#define KERNEL_VERSION "0.0.2"
#endif

#define KERNEL_DATE __DATE__
#define KERNEL_TIME __TIME__

#ifndef GIT_HASH
#define GIT_HASH "dirty"
#endif

#endif
