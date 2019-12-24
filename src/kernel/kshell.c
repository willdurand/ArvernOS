#include "kshell.h"
#include <core/cmos.h>
#include <core/debug.h>
#include <core/elf.h>
#include <core/timer.h>
#include <drivers/screen.h>
#include <fs/debug.h>
#include <fs/vfs.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

unsigned char keymap[][128] = {
    {0},
    {27},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {'\b'},
    {'\t'},
    {'q', 'Q'},
    {'w', 'W'},
    {'e', 'E'},
    {'r', 'R'},
    {'t', 'T'},
    {'y', 'Y'},
    {'u', 'U'},
    {'i', 'I'},
    {'o', 'O'},
    {'p', 'P'},
    {'[', '{'},
    {']', '}'},
    {'\n'},
    {0},
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {';', ':'},
    {'\'', '"'},
    {'`', '~'},
    {0}, // Left shift
    {'\\', '|'},
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {0}, // Right shift
    {'*'},
    {0}, // Alt
    {' '}, // Space
    {0}, // Caps lock
    {0}, // F1
    {0}, // F2
    {0}, // F3
    {0}, // F4
    {0}, // F5
    {0}, // F6
    {0}, // F8
    {0}, // F7
    {0}, // F9
    {0}, // F10
    {0}, // Num lock
    {0}, // Scroll lock
    {0}, // Home key
    {0}, // Arrow up
    {0}, // Page up
    {'-'},
    {0}, // Arrow left
    {0},
    {0}, // Arrow right
    {'+'},
    {0}, // End
    {0}, // Arrow down
    {0}, // Page down
    {0}, // Insert ket
    {0}, // Deltee key
    {0},
    {0},
    {0},
    {0}, // F11
    {0}, // F12
    {0},
};

#define NB_DOCUMENTED_COMMANDS 6

const char* commands[][NB_DOCUMENTED_COMMANDS] = {
    {"cat", "print on the standard output"},
    {"clear", "clear the terminal screen"},
    {"date", "print the system date and time"},
    {"help", "display information about willOS shell commands"},
    {"ls", "list files"},
    {"selftest", "run the willOS test suite"},
    {"uptime", "tell how long the system has been running"},
};

unsigned char get_char(uint8_t scancode, bool shift, bool caps_lock) {
    if ((caps_lock || shift) && keymap[scancode][1]) {
        return keymap[scancode][1];
    }

    return keymap[scancode][0];
}

void help(const char* command) {
    if (strlen(command) == 4) {
        for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
            printf("%-10s %s\n", commands[i][0], commands[i][1]);
        }

        return;
    }

    const char* arg = command + 5;

    for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
        if (strncmp(arg, commands[i][0], strlen(commands[i][0])) == 0) {
            printf("%s - %s\n", arg, commands[i][1]);
            return;
        }
    }

    printf("no help for this command\n");
}

void date() {
    cmos_rtc_t rtc = cmos_read_rtc();

    printf("%4d-%02d-%02d %02d:%02d:%02d UTC\n",
           rtc.year,
           rtc.month,
           rtc.day,
           rtc.hours,
           rtc.minutes,
           rtc.seconds
          );
}

void clear() {
    screen_clear();
}

void uptime() {
    printf("up %u seconds\n", timer_uptime());
}

void print_selftest_header(const char* name) {
    screen_color_scheme(COLOR_BROWN, COLOR_BLACK);
    printf("\n[%s]\n", name);
    screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
}

void selftest() {
    print_selftest_header("interrupts");
    printf("  invoking breakpoint exception\n");
    __asm__("int3");

    print_selftest_header("syscalls");
    printf("  syscalling\n");
    test("kshell");

    print_selftest_header("memory");
    char* str = 0x42;
    printf("    pointer before malloc(): p=%p\n", str);
    int str_len = 9;
    str = (char*)malloc(str_len * sizeof(char));

    if (str == 0) {
        printf("  failed\n");
    } else {
        printf("  success! p=%p", str);
        strncpy(str, "it works", str_len);
        printf(" and value is: %s\n", str);
        free(str);
    }

    print_selftest_header("filesystem");
    inode_t debug = vfs_namei(FS_DEBUG_FILENAME);
    const char* message = "  this message should be written to the console\n";
    vfs_write(debug, message, strlen(message), 0);

    printf("\ndone.\n");
}

void cat(const char* command) {
    const char* arg = command + 4;
    inode_t f = vfs_namei(arg);

    if (!f) {
        printf("no such file or directory\n");
        return;
    }

    if (f->type != FS_FILE) {
        printf("'%s' is not a printable file\n", f->name);
        return;
    }

    char buf[512];
    vfs_read(f, &buf, sizeof(buf), 0);
    printf("%s", buf);
}

void ls(const char* command) {
    const char* arg = command + 3;
    inode_t inode = strlen(arg) == 0 ? vfs_namei("/") : vfs_namei(arg);

    if (!inode) {
        printf("no such file or directory\n");
        return;
    }

    if (vfs_inode_type(inode) != FS_DIRECTORY) {
        printf("'%s' is not a directory\n", inode->name);
        vfs_free(inode);
        return;
    }

    uint64_t num = 0;

    while (1) {
        dirent_t* de = vfs_readdir(inode, num++);

        if (!de) {
            break;
        }

        stat_t stat;
        vfs_stat(de->inode, &stat);
        printf("%6d %s\n", stat.size, de->name);

        free(de);
    }

    vfs_free(inode);
}

int try_exec(const char* command) {
    inode_t inode = vfs_namei(command);

    if (!inode) {
        return -1;
    }

    if (vfs_inode_type(inode) != FS_FILE) {
        vfs_free(inode);
        return -2;
    }

    stat_t stat;
    vfs_stat(inode, &stat);

    char* buf = malloc((stat.size + 1) * sizeof(char));
    uint64_t bytes_read = vfs_read(inode, buf, stat.size, 0);

    DEBUG("bytes_read=%u", bytes_read);

    elf_header_t* elf = elf_load(buf);

    if (elf) {
        typedef int callable(void);
        callable* c = (callable*)(elf->entry);
        c();
    }

    free(buf);

    return 0;
}

void run_command(const char* command) {
    DEBUG("command='%s'", command);

    if (*command == 0) {
        return;
    }

    // TODO: implement and use `strktok()` to get the command and the arguments.

    if (strncmp(command, "help", 4) == 0) {
        help(command);
    } else if (strncmp(command, "ls", 2) == 0) {
        ls(command);
    } else if (strncmp(command, "cat", 3) == 0) {
        cat(command);
    } else if (strncmp(command, "date", 4) == 0) {
        date();
    } else if (strncmp(command, "clear", 5) == 0) {
        clear();
    } else if (strncmp(command, "uptime", 6) == 0) {
        uptime();
    } else if (strncmp(command, "selftest", 8) == 0) {
        selftest();
    } else {
        if (!try_exec(command)) {
            printf("invalid command\n");
        }
    }
}

void kshell_print_prompt() {
    screen_color_scheme(COLOR_CYAN, COLOR_BLACK);
    printf(PROMPT);
    screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
}

char readline[READLINE_SIZE] = {0};
char last_readline[READLINE_SIZE] = {0};
unsigned int readline_index = 0;

bool caps_lock_mode = false;
bool ctrl_mode = false;
bool shift_mode = false;

void reset_readline() {
    readline_index = 0;

    for (unsigned int i = 0; i < READLINE_SIZE; i++) {
        readline[i] = 0;
    }
}

void kshell_run(uint8_t scancode) {
    bool key_was_released = false;

    if (scancode > 128) {
        key_was_released = true;
        scancode -= 128;
    }

    switch (scancode) {
        case KB_CAPS_LOCK:
            if (key_was_released) {
                caps_lock_mode = !caps_lock_mode;
            }

            break;

        case KB_ARROW_UP:
            if (key_was_released) {
                reset_readline();
                strncpy(readline, last_readline, READLINE_SIZE);
                printf(readline);
                readline_index = strlen(readline);
            }

            break;

        case KB_LSHIFT:
        case KB_RSHIFT:
            if (key_was_released) {
                shift_mode = false;
            } else {
                shift_mode = true;
            }

            break;

        case KB_LCTRL:
            if (key_was_released) {
                ctrl_mode = false;
            } else {
                ctrl_mode = true;
            }

            break;

        case KB_BACKSPACE:
            if (key_was_released && readline_index > 0) {
                printf("\b");
                readline_index--;
                readline[readline_index] = 0;
            }

            break;

        case KB_ENTER:
            if (key_was_released) {
                printf("\n");
                run_command((const char*)readline);
                strncpy(last_readline, readline, READLINE_SIZE);
                reset_readline();
                kshell_print_prompt();
            }

            break;

        case KB_TAB:
            if (key_was_released) {
                printf("  ");
                readline[readline_index++] = 0;
                readline[readline_index++] = 0;
            }

            break;

        default:
            if (key_was_released) {
                unsigned char c = get_char(scancode, shift_mode, caps_lock_mode);

                if (c) {
                    // Handle keyboard shortcuts.
                    if (ctrl_mode) {
                        switch (c) {
                            case 'c':
                                readline[readline_index++] = '^';
                                readline[readline_index++] = 'C';
                                printf("^C\n");
                                kshell_print_prompt();
                                break;

                            case 'l':
                                clear();
                                reset_readline();
                                kshell_print_prompt();
                                break;
                        }
                    } else {
                        printf("%c", c);
                        readline[readline_index++] = c;
                    }
                }
            }
    }
}
