/** @file */
#ifndef OSINFO_H
#define OSINFO_H

#define KERNEL_ASCII                                                           \
  "    ___                               ____  _____\n"                        \
  "   /   |  ______   _____  _________  / __ \\/ ___/\n"                       \
  "  / /| | / ___/ | / / _ \\/ ___/ __ \\/ / / /\\__ \\\n"                     \
  " / ___ |/ /   | |/ /  __/ /  / / / / /_/ /___/ /\n"                         \
  "/_/  |_/_/    |___/\\___/_/  /_/ /_/\\____//____/\n"

#define KERNEL_NAME "ArvernOS"

#ifdef ENABLE_KERNEL_DEBUG
#define KERNEL_VERSION "DEBUG MODE"
#else
#define KERNEL_VERSION "0.0.3"
#endif

#define KERNEL_DATE __DATE__
#define KERNEL_TIME __TIME__

#ifndef GIT_HASH
#define GIT_HASH "dirty"
#endif

#endif
