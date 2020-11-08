#ifndef CONFIG_H
#define CONFIG_H

#define BLACK         "\x1B[30m"
#define GREY          "\x1b[1;30m"
#define RED           "\x1B[31m"
#define LIGHT_RED     "\x1B[1;31m"
#define GREEN         "\x1B[32m"
#define LIGHT_GREEN   "\x1B[1;32m"
#define YELLOW        "\x1B[33m"
#define LIGHT_YELLOW  "\x1B[1;33m"
#define BLUE          "\x1B[34m"
#define LIGHT_BLUE    "\x1B[1;34m"
#define MAGENTA       "\x1B[35m"
#define LIGHT_MAGENTA "\x1B[1;35m"
#define CYAN          "\x1B[36m"
#define LIGHT_CYAN    "\x1B[1;36m"
#define WHITE         "\x1B[37m"
#define LIGHT_WHITE   "\x1B[1;37m"
//#define RESET          "\x1B[0m"
#define RESET ""

// Normal text color
const char normal[] = ""; // WHITE;

// Dark text color, used for day tags
const char darken[] = ""; // LIGHT_RED;

// Active color, for time, and current day indication
const char active[] = ""; // LIGHT_GREEN;

// Time & date format,
// SEE strftime MANPAGE FOR MORE EXPANSIONS
// http://man7.org/linux/man-pages/man3/strftime.3.html

// Time format, by default 'HH:MM'
const char cdate_fmt0[] = "It's %H:%M o'clock!";

// Date format, by default 'Day. XX Month Year'
const char cdate_fmt1[] = "%a. %d %B %Y";

#endif
