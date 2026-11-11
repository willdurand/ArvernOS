#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

/// Maximum length of a program path (including the `NULL` terminator).
#define PROGRAM_PATH_SIZE 128

void run(char* argv[])
{
  char path[PROGRAM_PATH_SIZE];

  // A name that contains a slash is a path; anything else is looked up in
  // `/bin`.
  if (strchr(argv[0], '/') != NULL) {
    snprintf(path, sizeof(path), "%s", argv[0]);
  } else {
    snprintf(path, sizeof(path), "/bin/%s", argv[0]);
  }

  pid_t pid = fork();
  if (pid < 0) {
    printf("%s: cannot fork (%d)\n", argv[0], (int)pid);
    return;
  }

  if (pid == 0) {
    // Load the program, maybe.
    execv(path, argv);

    printf("%s: command not found\n", argv[0]);
    exit(127);
  }

  int status = 0;
  if (waitpid(pid, &status) < 0) {
    printf("%s: waitpid failed\n", argv[0]);
  }
}
