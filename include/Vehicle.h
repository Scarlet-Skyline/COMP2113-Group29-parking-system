// Vehicle.h
// COMP2113 Group Project: Parking Lot Management System
//
// A vehicle in the car park: its plate and its entry and exit times.  Create
// vehicles with makeVehicle() so that the plate is always normalised and valid.

#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include "TimeUtils.h"

/**
 * @brief One vehicle and its parking times.
 */
struct Vehicle{
    std::string plate;  //!< Upper-case, valid plate when the vehicle was made by makeVehicle().
    DateTime EntryTime; //!< When the vehicle entered the car park.
    DateTime ExitTime;  //!< When it left.  All fields are 0 until the vehicle exits.
};

/**
 * @brief Build a Vehicle from user input, rejecting an invalid plate.
 *
 * The plate is converted with normalize_plate() and then checked with
 * validate_plate().  The entry time is stored as given and is not validated.
 * ExitTime is set to all zeros (day, month, year, hour and minute).
 *
 * @param rawPlate Plate as typed by the user, in any letter case.
 * @param entry    Time of entry, stored as EntryTime.
 * @param out      Receives the new Vehicle on success.  Left unchanged if the
 *                 plate is invalid.
 * @return true if the plate is valid and @p out was filled, false otherwise.
 */
bool makeVehicle(const std::string &rawPlate, const DateTime &entry, Vehicle &out);

#endif