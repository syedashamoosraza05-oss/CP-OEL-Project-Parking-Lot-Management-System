#ifndef DATE_TIME_UTILS_H
#define DATE_TIME_UTILS_H

int daysInMonth(int month, int year);
void readDate(const char title[], int *day, int *month, int *year);
void readTime(const char title[], int *hour, int *min);
long dayNumber(int day, int month, int year);
long toMinutes(int day, int month, int year, int hour, int min);

#endif
