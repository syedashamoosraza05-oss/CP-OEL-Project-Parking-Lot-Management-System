#include <stdio.h>
#include "date_time_utils.h"
#include "input_utils.h"      /* readInt, readString and the colour macros */

/* Number of days in a month (handles leap years). */
int daysInMonth(int month, int year) {
    int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
        return 29;
    return d[month - 1];
}

/* Asks for a date in DD/MM/YYYY format and checks that it is a real date. */
void readDate(const char title[], int *day, int *month, int *year) {
    char buf[30];
    int d, m, y;
    printf(CYAN "%s\n" RESET, title);
    while (1) {
        readString("  Date (DD/MM/YYYY): ", buf, sizeof(buf));
        if (sscanf(buf, "%d/%d/%d", &d, &m, &y) == 3 &&
            y >= 2000 && y <= 2100 && m >= 1 && m <= 12 &&
            d >= 1 && d <= daysInMonth(m, y)) {
            *day = d;
            *month = m;
            *year = y;
            return;
        }
        printf(RED "  Invalid date. Use DD/MM/YYYY, e.g. 17/11/2026.\n" RESET);
    }
}

/* Asks for a time: hour and minute. */
void readTime(const char title[], int *hour, int *min) {
    printf(CYAN "%s\n" RESET, title);
    *hour = readInt("  Hour (0-23): ", 0, 23);
    *min  = readInt("  Minute (0-59): ", 0, 59);
}

/* Converts a date to a running day number so two dates can be subtracted. */
long dayNumber(int day, int month, int year) {
    long total = 0;

    /* 1. add all the full years before this one */
    for (int y = 1; y < year; y++) {
        for (int m = 1; m <= 12; m++)
            total += daysInMonth(m, y);
    }

    /* 2. add all the full months before this month */
    for (int m = 1; m < month; m++)
        total += daysInMonth(m, year);

    /* 3. add the days in the current month */
    return total + day;
}

/* Total minutes since a fixed starting point (used for the stay length). */
long toMinutes(int day, int month, int year, int hour, int min) {
    return dayNumber(day, month, year) * 1440L + hour * 60 + min;
}
