/**
 * @file
 * @see https://wiki.osdev.org/Reboot
 * @see http://man7.org/linux/man-pages/man2/reboot.2.html
 */
#ifndef CORE_REBOOT_H
#define CORE_REBOOT_H

/**
 * Reboots the system.
 *
 * @param command a command value (defined in `<sys/reboot.h>`).
 * @return depending on the command, either this function does not return or it
 * returns 0 on success and -1 on error (with `errno` set appropriately).
 */
int kreboot(int command);

#endif
