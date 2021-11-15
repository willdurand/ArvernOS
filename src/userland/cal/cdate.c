#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cdate.h"
#include "config.h"

// !---
void cdate()
{
  t_cdate cdate;

  cdate_init(&cdate);
  cdate_str(&cdate);
  cdate_daytag(&cdate);
  cdate_calendar(&cdate);
}

// !---
void cdate_init(t_cdate* cdate)
{
  time(&(cdate->time));
  cdate->tm = localtime(&(cdate->time));
}

// !---
void cdate_str(t_cdate* cdate)
{
  char str0[64];
  char str1[64];

  memset(str0, '\0', sizeof(str0));
  memset(str1, '\0', sizeof(str1));
  strftime(str0, sizeof(str0), cdate_fmt0, cdate->tm);
  strftime(str1, sizeof(str1), cdate_fmt1, cdate->tm);
  putchar('\n');

  printf(normal);
  printf("%*s\n", (int)strlen(str0) + 3, str0);
  printf("%*s\n", (int)strlen(str1) + 3, str1);
  printf(RESET);
}

// !---
static const char* const daytag_en[] = {
  "m", "t", "w", "t", "f", "s", "S", NULL
};
void cdate_daytag(t_cdate* cdate)
{
  int today;
  int i;
  char** daytags;

  printf(normal);
  daytags = (char**)daytag_en;
  putchar('\n');
  today = cdate->tm->tm_wday - 1;
  today < 0 ? today = 6 : (0);
  i = 0;

  while (*daytags != NULL) {
    printf(darken);

    if (i == today) {
      printf(active);
    }

    printf("%4s", *daytags);
    printf(RESET);
    ++daytags;
    ++i;
  }

  printf(RESET);
  putchar('\n');
}

// !---
void cdate_calendar(t_cdate* cdate)
{
  int padding;
  int off;
  int daycount;
  int col;
  int i;
  int today;

  printf(normal);
  daycount = cdate_daycount(cdate);
  padding = cdate->tm->tm_wday;
  off = cdate->tm->tm_mday;
  col = 0;
  i = 0;

  while (off--) {
    --padding;

    if (padding < 0) {
      padding = 6;
    }
  }

  while (padding--) {
    ++col;
    printf("%4c", ' ');
  }

  today = cdate->tm->tm_mday - 1;

  if (today < 0) {
    today = daycount - 1;
  }

  while (i < daycount) {
    printf(normal);

    if (i == today) {
      printf(active);
    }

    printf("%4i", i + 1);
    printf(RESET);
    ++i;
    ++col;

    if (col == 7) {
      col = 0;
      putchar('\n');
    }
  }

  putchar('\n');
  putchar('\n');
}

// !---
int cdate_daycount(t_cdate* cdate)
{
  int count;
  int month;
  int year;
  int is_leap;

  month = cdate->tm->tm_mon + 1;

  if (month == 4 || month == 6 || month == 9 || month == 11) {
    count = 30;
  } else if (month == 2) {
    year = cdate->tm->tm_year;
    is_leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    count = is_leap ? 29 : 28;
  } else {
    count = 31;
  }

  return (count);
}
