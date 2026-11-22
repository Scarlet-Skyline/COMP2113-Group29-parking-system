#ifndef FEECALCULATOR_H
#define FEECALCULATOR_H

#include <string>
#include <ctime>

const int HOUR = 60;
const int FeePerHour = 20;
const int FeePerHourWeekend = 24;
const int MINIMUM_PURCHASE_WEEKDAY = 300;
const int MINIMUM_PURCHASE_WEEKEND = 400;
const int WEEKDAY_FREE_HOURS = 2;
const int WEEKDAY_FREE_HOURS_EVENING = 4;
const int WEEKEND_FREE_HOURS = 2;

struct DateTime {
    int day, month, year;
    int hour, minute;
};

struct Scenario {
    int duration; //in hours
    bool isWeekday;
    bool isEvening;
};

DateTime parseDateTime(const std::string &date, const std::string &timeStr);
Scenario calculateDuration(DateTime entrance_time, DateTime exit_time);
int calculateAmount(Scenario s, double purchase);

#endif