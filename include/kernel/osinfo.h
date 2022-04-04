/**
 * @file
 *
 * This file contains information about the kernel such as its name, version,
 * etc.
 */
#ifndef OSINFO_H
#define OSINFO_H

#define KERNEL_ASCII                                                           \
  "    ___                               ____  _____\n"                        \
  "   /   |  ______   _____  _________  / __ \\/ ___/\n"                       \
  "  / /| | / ___/ | / / _ \\/ ___/ __ \\/ / / /\\__ \\\n"                     \
  " / ___ |/ /   | |/ /  __/ /  / / / / /_/ /___/ /\n"                         \
  "/_/  |_/_/    |___/\\___/_/  /_/ /_/\\____//____/\n"

#define KERNEL_NAME "ArvernOS"

#define _KERNEL_VERSION "0.0.4"

#ifdef ENABLE_KERNEL_DEBUG
#define KERNEL_VERSION _KERNEL_VERSION "-debug"
#else
#define KERNEL_VERSION _KERNEL_VERSION
#endif

#define _KERNEL_TARGET ARCH

#ifdef BOARD
#define KERNEL_TARGET ARCH "+" BOARD
#else
#define KERNEL_TARGET ARCH
#endif

#define KERNEL_DATE __DATE__
#define KERNEL_TIME __TIME__

#ifndef KERNEL_GIT_HASH
#define KERNEL_GIT_HASH "dirty"
#endif

#endif
