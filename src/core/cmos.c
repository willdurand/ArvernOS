#include "cmos.h"
#include <core/port.h>
#include <stdbool.h>
#include <string.h>

uint8_t read_register(uint8_t reg);
bool update_in_progress();
bool rtc_values_are_not_equal(cmos_rtc_t c1, cmos_rtc_t c2);
uint64_t secs_of_month(uint64_t months, uint64_t year);
uint64_t secs_of_years(uint64_t years);

uint64_t boot_time = 0;

void cmos_init()
{
  cmos_rtc_t rtc = cmos_read_rtc();
  boot_time = secs_of_years(rtc.year - 1) +
              secs_of_month(rtc.month - 1, rtc.year) + (rtc.day - 1) * 86400 +
              rtc.hours * 3600 + rtc.minutes * 60 + rtc.seconds;
}

uint64_t cmos_boot_time()
{
  return boot_time;
}

cmos_rtc_t cmos_read_rtc()
{
  cmos_rtc_t rtc;
  cmos_rtc_t last;

  // This uses the "read registers until you get the same values twice in a
  // row" technique to avoid getting inconsistent values due to RTC updates
  while (update_in_progress())
    ;

  // read a first time
  rtc.seconds = read_register(CMOS_REG_SECONDS);
  rtc.minutes = read_register(CMOS_REG_MINUTES);
  rtc.hours = read_register(CMOS_REG_HOURS);
  rtc.weekdays = read_register(CMOS_REG_WEEKDAYS);
  rtc.day = read_register(CMOS_REG_DAY);
  rtc.month = read_register(CMOS_REG_MONTH);
  rtc.year = read_register(CMOS_REG_YEAR);
  rtc.century = read_register(CMOS_REG_CENTURY);

  do {
    // prepare to read a second time
    memcpy(&last, &rtc, sizeof(cmos_rtc_t));

    while (update_in_progress())
      ;

    // read a second time
    rtc.seconds = read_register(CMOS_REG_SECONDS);
    rtc.minutes = read_register(CMOS_REG_MINUTES);
    rtc.hours = read_register(CMOS_REG_HOURS);
    rtc.weekdays = read_register(CMOS_REG_WEEKDAYS);
    rtc.day = read_register(CMOS_REG_DAY);
    rtc.month = read_register(CMOS_REG_MONTH);
    rtc.year = read_register(CMOS_REG_YEAR);
    rtc.century = read_register(CMOS_REG_CENTURY);
  } while (rtc_values_are_not_equal(rtc, last));

  // Status Register B contains the formats of bytes
  uint8_t reg_b = read_register(CMOS_REG_STATUS_B);

  if (!(reg_b & 0x04)) {
    // convert BCD back into a "good" binary value
    rtc.seconds = (rtc.seconds & 0x0F) + ((rtc.seconds / 16) * 10);
    rtc.minutes = (rtc.minutes & 0x0F) + ((rtc.minutes / 16) * 10);
    rtc.hours = ((rtc.hours & 0x0F) + (((rtc.hours & 0x70) / 16) * 10)) |
                (rtc.hours & 0x80);
    rtc.weekdays = (rtc.weekdays & 0x0F) + ((rtc.weekdays / 16) * 10);
    rtc.day = (rtc.day & 0x0F) + ((rtc.day / 16) * 10);
    rtc.month = (rtc.month & 0x0F) + ((rtc.month / 16) * 10);
    rtc.year = (rtc.year & 0x0F) + ((rtc.year / 16) * 10);
    rtc.century = (rtc.century & 0x0F) + ((rtc.century / 16) * 10);
  }

  // if the hour is pm, then the 0x80 bit is set on the hour byte
  if (!(reg_b & 0x02) && (rtc.hours & 0x80)) {
    // Convert 12 hour clock to 24 hour clock if necessary
    rtc.hours = ((rtc.hours & 0x7F) + 12) % 24;
  }

  // compute full year
  rtc.year += (rtc.century * 100);

  return rtc;
}

bool update_in_progress()
{
  port_byte_out(CMOS_COMMAND_PORT, CMOS_REG_STATUS_A);
  // the RTC has an "Update in progress" flag (bit 7 of Status Register A)
  return (port_byte_in(CMOS_DATA_PORT) & 0x80);
}

uint8_t read_register(uint8_t reg)
{
  port_byte_out(CMOS_COMMAND_PORT, (1 << 7) | reg);
  return port_byte_in(CMOS_DATA_PORT);
}

bool rtc_values_are_not_equal(cmos_rtc_t c1, cmos_rtc_t c2)
{
  return (c1.seconds != c2.seconds || c1.minutes != c2.minutes ||
          c1.hours != c2.hours || c1.weekdays != c2.weekdays ||
          c1.day != c2.day || c1.month != c2.month || c1.year != c2.year ||
          c1.century != c2.century);
}

uint64_t secs_of_years(uint64_t years)
{
  uint64_t days = 0;

  while (years > 1969) {
    days += 365;

    if (years % 4 == 0) {
      if (years % 100 == 0) {
        if (years % 400 == 0) {
          days++;
        }
      } else {
        days++;
      }
    }

    years--;
  }

  return days * 86400;
}

uint64_t secs_of_month(uint64_t months, uint64_t year)
{
  uint64_t days = 0;

  switch (months) {
    case 11:
      days += 30;

    case 10:
      days += 31;

    case 9:
      days += 30;

    case 8:
      days += 31;

    case 7:
      days += 31;

    case 6:
      days += 30;

    case 5:
      days += 31;

    case 4:
      days += 30;

    case 3:
      days += 31;

    case 2:
      days += 28;

      if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0))) {
        days++;
      }

    case 1:
      days += 31;

    default:
      break;
  }

  return days * 86400;
}
