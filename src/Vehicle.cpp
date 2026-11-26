#include "Vehicle.h"
#include "PlateValidator.h"

bool makeVehicle(const std::string &rawPlate, const DateTime &entry, Vehicle &out){
    std::string plate = normalize_plate(rawPlate);
    if (!validate_plate(plate)) return false;
    out.plate = plate;
    out.EntryTime = entry;
    out.ExitTime = DateTime{};
    return true;
}

