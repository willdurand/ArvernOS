#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void env()
{
  int i = 0;
  while (environ[i] != NULL) {
    printf("%s\n", environ[i]);
    i++;
  }
}
