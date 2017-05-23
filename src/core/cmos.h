#ifndef CORE_CMOS_H
#define CORE_CMOS_H

#include <stdint.h>

#define CMOS_COMMAND_PORT   0x70
#define CMOS_DATA_PORT      0x71

// cf. http://wiki.osdev.org/CMOS
#define CMOS_REG_SECONDS        0x00
#define CMOS_REG_MINUTES        0x02
#define CMOS_REG_HOURS          0x04
#define CMOS_REG_WEEKDAYS       0x06
#define CMOS_REG_DAY            0x07
#define CMOS_REG_MONTH          0x08
#define CMOS_REG_YEAR           0x09
#define CMOS_REG_CENTURY        0x32
#define CMOS_REG_STATUS_A       0x0A
#define CMOS_REG_STATUS_B       0x0B

typedef struct cmos_rtc {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t weekdays;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t century;
} cmos_rtc_t;

cmos_rtc_t cmos_read_rtc();

#endif
