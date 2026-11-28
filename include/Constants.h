// Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Admin authentication
constexpr const char *ADMIN_PASSWORD = "COMP2113_G29_Parking";

// Persistence / main
constexpr const char *DATA_FILE = "parking_data.txt";
constexpr int DEFAULT_CAPACITY = 10;

// FeeCalculator constants (moved out of the header so there is only one
// definition to reason about)
constexpr int HOUR = 60;
constexpr int FeePerHour = 20;
constexpr int FeePerHourWeekend = 24;
constexpr int MINIMUM_PURCHASE_WEEKDAY = 300;
constexpr int MINIMUM_PURCHASE_WEEKEND = 400;
constexpr int WEEKDAY_FREE_HOURS = 2;
constexpr int WEEKDAY_FREE_HOURS_EVENING = 4;
constexpr int WEEKEND_FREE_HOURS = 2;

#endif // CONSTANTS_H