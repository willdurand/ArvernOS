#ifndef CDATE_H
#define CDATE_H

#include <time.h>

struct s_cdate
{
  time_t time;
  struct tm* tm;
};

typedef struct s_cdate t_cdate;

void cdate();
void cdate_init(t_cdate*);
void cdate_str(t_cdate*);
void cdate_daytag(t_cdate*);
void cdate_calendar(t_cdate*);
int cdate_daycount(t_cdate*);

#endif
