#ifndef FEECALCULATOR_H
#define FEECALCULATOR_H

#include "TimeUtils.h"
#include "Constants.h"

struct Scenario {
    int duration; //in hours
    bool isWeekday;
    bool isEvening;
};

Scenario calculateDuration(DateTime entrance_time, DateTime exit_time);
int calculateAmount(Scenario s, double purchase);

#endif