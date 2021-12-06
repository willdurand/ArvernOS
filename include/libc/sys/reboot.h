/** @file */
#ifndef SYS_REBOOT_H
#define SYS_REBOOT_H

/// A message is printed, and a default restart is performed immediately.
#define REBOOT_CMD_RESTART 2
/// A message is printed, and the system is stopped and powered off.
#define REBOOT_CMD_POWER_OFF 3

#ifdef __is_libk
/// This is like `REBOOT_CMD_POWER_OFF` but it sets the exit code to `0`
/// (success) when `CONFIG_SEMIHOSTING` is set to `1` and the emulator supports
/// exit codes.
#define REBOOT_CMD_POWER_OFF_EXIT_CODE_0 100
/// This is like `REBOOT_CMD_POWER_OFF` but it sets the exit code to `1` (error)
/// when `CONFIG_SEMIHOSTING` is set to `1` and the emulator supports exit
/// codes.
#define REBOOT_CMD_POWER_OFF_EXIT_CODE_1 101
#endif

#endif
