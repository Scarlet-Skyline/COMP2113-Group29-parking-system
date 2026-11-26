#include "FeeCalculator.h"

#include <cmath>
#include <string>
#include <ctime>

Scenario calculateDuration(DateTime entrance_time, DateTime exit_time){
    Scenario S;
    double seconds = std::difftime(toTimeT(exit_time), toTimeT(entrance_time));
    S.duration = (int)std::ceil(seconds / 3600.0);

    int w = getWeekday(entrance_time);
    S.isWeekday = (w >= 1 && w <= 5);          // Monday to Friday

    S.isEvening = (entrance_time.hour >= 17);
    if (S.duration < 0) S.duration = 0;
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