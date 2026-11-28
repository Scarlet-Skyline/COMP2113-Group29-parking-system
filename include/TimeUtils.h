#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <ctime>
#include <string>

struct DateTime{
    int day, month, year;
    int hour, minute;
};

DateTime parseDateTime(const std::string &date, const std::string &timeStr);
std::time_t toTimeT(const DateTime &d);
int getWeekday(const DateTime &d);  // 0 = Sunday ... 6 = Saturday

DateTime getCurrentDateTime(); 

#endif