#include "TimeUtils.h"
#include <cstdio>
#include <string>
#include <ctime>

DateTime parseDateTime(const std::string &date, const std::string &timeStr){
    DateTime dt;
    std::sscanf(date.c_str(), "%d/%d/%d", &dt.day, &dt.month, &dt.year);
    int hhmm = std::stoi(timeStr);
    dt.hour = hhmm/100;
    dt.minute = hhmm%100;
    return dt;
}

std::time_t toTimeT(const DateTime &d){
    std::tm t = {};
    t.tm_year = d.year - 1900;
    t.tm_mon = d.month -1;
    t.tm_mday = d.day;
    t.tm_hour = d.hour;
    t.tm_min = d.minute;
    t.tm_isdst = -1;
    return std::mktime(&t);
}

int getWeekday(const DateTime &d){
    std::tm t = {};
    t.tm_year = d.year - 1900;
    t.tm_mon  = d.month - 1;
    t.tm_mday = d.day;
    t.tm_hour = 12;
    t.tm_isdst = -1;
    std::mktime(&t);
    return t.tm_wday;
}                 // 0 = Sunday... 6 = Saturday

DateTime getCurrentDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    DateTime d{};
    d.year   = tmv.tm_year + 1900;
    d.month  = tmv.tm_mon + 1;
    d.day    = tmv.tm_mday;
    d.hour   = tmv.tm_hour;
    d.minute = tmv.tm_min;
    return d;
}