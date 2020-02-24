/**
 * University of Illinois/NCSA Open Source License
 *
 * Copyright (c) 2011-2019 K Lange, et al. (hereafter [fullname]). All rights
 * reserved.
 *
 * Developed by: ToaruOS (hereafter [project])
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimers.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimers in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither the names of [fullname], [project] nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * Software without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */
#include <sys/syscall.h>
#include <time.h>

#define SEC_DAY 86400

static struct tm _timevalue;

static int year_is_leap(int year)
{
  return ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)));
}

// 0 was a Thursday

static int day_of_week(long seconds)
{
  long day = seconds / SEC_DAY;
  day += 4;
  return day % 7;
}

static long days_in_month(int month, int year)
{
  switch (month) {
    case 12:
      return 31;

    case 11:
      return 30;

    case 10:
      return 31;

    case 9:
      return 30;

    case 8:
      return 31;

    case 7:
      return 31;

    case 6:
      return 30;

    case 5:
      return 31;

    case 4:
      return 30;

    case 3:
      return 31;

    case 2:
      return year_is_leap(year) ? 29 : 28;

    case 1:
      return 31;
  }

  return 0;
}

struct tm* localtime_r(const time_t* timep, struct tm* _timevalue)
{

  long seconds = 0; // this needs to be bigger, but whatever

  long year_sec = 0;

  for (int year = 1970; year < 2100; ++year) {
    long added = year_is_leap(year) ? 366 : 365;
    long secs = added * 86400;

    if (seconds + secs >= *timep + 1) {
      _timevalue->tm_year = year - 1900;
      year_sec = seconds;

      for (int month = 1; month <= 12; ++month) {
        secs = days_in_month(month, year) * SEC_DAY;

        if (seconds + secs >= *timep) {
          _timevalue->tm_mon = month - 1;

          for (int day = 1; day <= days_in_month(month, year); ++day) {
            secs = 60 * 60 * 24;

            if (seconds + secs >= *timep) {
              _timevalue->tm_mday = day;

              for (int hour = 1; hour <= 24; ++hour) {
                secs = 60 * 60;

                if (seconds + secs >= *timep) {
                  long remaining = *timep - seconds;
                  _timevalue->tm_hour = hour - 1;
                  _timevalue->tm_min = remaining / 60;
                  _timevalue->tm_sec =
                    remaining % 60; // can be 60 on a leap second, ignore that
                  _timevalue->tm_wday = day_of_week(*timep); // oh shit
                  _timevalue->tm_yday = (*timep - year_sec) / SEC_DAY;
                  _timevalue->tm_isdst = 0; // never because UTC
                  return _timevalue;
                }

                seconds += secs;
              }

              return NULL;
            }

            seconds += secs;
          }

          return NULL;
        }

        seconds += secs;
      }

      return NULL;
    }

    seconds += secs;
  }

  return (void*)0; // uh what
}

struct tm* localtime(const time_t* time_ptr)
{
  return localtime_r(time_ptr, &_timevalue);
}
