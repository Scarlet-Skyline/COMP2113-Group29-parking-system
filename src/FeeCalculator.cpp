#include "FeeCalculator.h"

#include <cmath>
#include <string>
#include <cstdio>
#include <ctime>

DateTime parseDateTime(const std::string &date, const std::string &timeStr){
    DateTime dt;
    std::sscanf(date.c_str(), "%d/%d/%d", &dt.day, &dt.month, &dt.year);
    int hhmm = std::stoi(timeStr);
    dt.hour = hhmm/100;
    dt.minute = hhmm%100;
    return dt;
}

static std::time_t toTimeT(const DateTime &d){
    std::tm t = {};
    t.tm_year = d.year - 1900;
    t.tm_mon = d.month -1;
    t.tm_mday = d.day;
    t.tm_hour = d.hour;
    t.tm_min = d.minute;
    t.tm_isdst = -1;
    return std::mktime(&t);
}

static int getWeekday(const DateTime &d){
    std::tm t = {};
    t.tm_year = d.year - 1900;
    t.tm_mon  = d.month - 1;
    t.tm_mday = d.day;
    t.tm_hour = 12;
    t.tm_isdst = -1;
    std::mktime(&t);
    return t.tm_wday;                 // 0 = Sunday ... 6 = Saturday
}

Scenario calculateDuration(DateTime entrance_time, DateTime exit_time){
    Scenario S;
    double seconds = std::difftime(toTimeT(exit_time), toTimeT(entrance_time));
    S.duration = (int)std::ceil(seconds / 3600.0);

    int w = getWeekday(entrance_time);
    S.isWeekday = (w >= 1 && w <= 5);          // Monday to Friday

    S.isEvening = (entrance_time.hour >= 17);
    return S;
}

int calculateAmount(Scenario s, double purchase){
    int dur = s.duration;
    int amount = 0;
    if (s.isWeekday){
        if (s.isEvening){   //weekday & evening
            if (purchase >= MINIMUM_PURCHASE_WEEKDAY){
                dur = dur - WEEKDAY_FREE_HOURS_EVENING;
                if (dur <= 0){
                    amount = 0;
                }
                else{
                    amount = dur * FeePerHour;
                }
            }
            else{
                amount = dur * FeePerHour;
            }
        }
        else{   //weekday & not evening
            if (purchase >= MINIMUM_PURCHASE_WEEKDAY){
                dur = dur - WEEKDAY_FREE_HOURS;
                if (dur <= 0){
                    amount = 0;
                }
                else{
                    amount = dur * FeePerHour;
                }
            }
            else{
                amount = dur * FeePerHour;
            }
        }
    }
    else{   //weekend
        if (purchase >= MINIMUM_PURCHASE_WEEKEND){
            dur = dur - WEEKEND_FREE_HOURS;
            if (dur <= 0){
                amount = 0;
            }
            else{
                amount = dur * FeePerHourWeekend;
            }
        }
        else{
            amount = dur * FeePerHourWeekend;
        }
    }
    return amount;
}