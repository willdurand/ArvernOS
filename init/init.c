#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cdate.h"

#define READLINE_SIZE           256
#define PROMPT                  "(init) "
#define NB_DOCUMENTED_COMMANDS  7

void date() {
    printf("implement me\n");
}

void clear() {
    printf("implement me\n");
}

void reboot() {
    printf("implement me\n");
}

void uptime() {
    printf("implement me\n");
}

int main() {
    const char* commands[][NB_DOCUMENTED_COMMANDS] = {
        {"cal", "displays a calendar"},
        {"clear", "clear the terminal screen"},
        {"date", "print the system date and time"},
        {"exit", "exit the shell"},
        {"help", "display information about willOS shell commands"},
        {"reboot", "stopping and restarting the system"},
        {"uptime", "tell how long the system has been running"},
    };

    char readline[READLINE_SIZE] = {0};
    unsigned int readline_index = 0;

    printf(PROMPT);

    while (1) {
        char c = getchar();

        switch (c) {
            case '\b':
                if (readline_index == 0) {
                    break;
                }

                printf("\b");

                if (readline_index >= 1) {
                    readline_index--;
                }

                readline[readline_index] = 0;

                break;

            case '\n':
                if (*readline == 0) {
                    break;
                }

                printf("\n");

                // TODO: implement and use `strktok()` to get the readline and
                // the arguments.

                if (strncmp(readline, "exit", 4) == 0) {
                    return 0;
                } else if (strncmp(readline, "help", 4) == 0) {
                    if (strlen(readline) == 4) {
                        for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
                            printf("%-10s %s\n", commands[i][0], commands[i][1]);
                        }
                    } else {
                        const char* arg = readline + 5;

                        for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
                            if (strncmp(arg, commands[i][0], strlen(commands[i][0])) == 0) {
                                printf("%s - %s\n", arg, commands[i][1]);
                                break;
                            }
                        }
                    }
                } else if (strncmp(readline, "cal", 3) == 0) {
                    cdate();
                } else if (strncmp(readline, "date", 4) == 0) {
                    date();
                } else if (strncmp(readline, "clear", 5) == 0) {
                    clear();
                } else if (strncmp(readline, "reboot", 6) == 0) {
                    reboot();
                } else if (strncmp(readline, "uptime", 6) == 0) {
                    uptime();
                } else {
                    printf("invalid readline\n");
                }

                readline_index = 0;

                for (unsigned int i = 0; i < READLINE_SIZE; i++) {
                    readline[i] = 0;
                }

                printf(PROMPT);

                break;

            case '\t':
                printf("  ");
                readline[readline_index++] = 0;
                readline[readline_index++] = 0;

                break;

            default:
                printf("%c", c);
                readline[readline_index++] = c;
        }
    }

    return 0;
}
