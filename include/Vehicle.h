#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include "TimeUtils.h"

struct Vehicle{
    std::string plate;
    DateTime EntryTime;
    DateTime ExitTime;
};

// Returns false (and leaves `out` untouched) if the plate is invalid.
bool makeVehicle(const std::string &rawPlate, const DateTime &entry, Vehicle &out);

#endif