// ParkingLot.cpp
// COMP2113 Group Project: Parking Lot Management System

#include "ParkingLot.h"

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "FeeCalculator.h"
#include "PlateValidator.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ParkingLot::ParkingLot(int capacity)
    : capacity_(capacity)
{
    if (capacity_ <= 0)
    {
        throw std::invalid_argument("ParkingLot: capacity must be positive");
    }
    parkedVehicles_.reserve(static_cast<std::size_t>(capacity_));
}

// ---------------------------------------------------------------------------
// Simple queries
// ---------------------------------------------------------------------------

int ParkingLot::capacity() const
{
    return capacity_;
}

int ParkingLot::parkedCount() const
{
    return static_cast<int>(parkedVehicles_.size());
}

int ParkingLot::remainingSpaces() const
{
    const int parked = static_cast<int>(parkedVehicles_.size());
    return capacity_ > parked ? capacity_ - parked : 0;
}

std::size_t ParkingLot::queueSize() const
{
    return waitingQueue_.size();
}

std::size_t ParkingLot::parkedIndex(const std::string &plate) const
{
    for (std::size_t i = 0; i < parkedVehicles_.size(); ++i)
    {
        if (parkedVehicles_[i]->plate == plate)
        {
            return i;
        }
    }
    return parkedVehicles_.size();
}

bool ParkingLot::isParked(const std::string &plate) const
{
    return parkedIndex(normalize_plate(plate)) < parkedVehicles_.size();
}

bool ParkingLot::isWaiting(const std::string &plate) const
{
    const std::string normalised = normalize_plate(plate);
    const std::vector<VehicleInfo> waiting = getWaitingSnapshot();

    for (std::size_t i = 0; i < waiting.size(); ++i)
    {
        if (waiting[i].plate == normalised)
        {
            return true;
        }
    }
    return false;
}

std::vector<VehicleInfo> ParkingLot::getParkedSnapshot() const
{
    std::vector<VehicleInfo> snapshot;
    snapshot.reserve(parkedVehicles_.size());

    for (std::size_t i = 0; i < parkedVehicles_.size(); ++i)
    {
        VehicleInfo info;
        info.plate = parkedVehicles_[i]->plate;
        info.hour = parkedVehicles_[i]->EntryTime.hour;
        snapshot.push_back(info);
    }

    return snapshot;
}

std::vector<VehicleInfo> ParkingLot::getWaitingSnapshot() const
{
    std::vector<VehicleInfo> snapshot;
    const std::size_t count = waitingQueue_.size();
    snapshot.reserve(count);

    // Rotate the whole queue once: move each vehicle out, remember it, push it
    // back.  The queue ends in exactly the same order.
    for (std::size_t i = 0; i < count; ++i)
    {
        std::unique_ptr<Vehicle> vehicle = std::move(waitingQueue_.front());
        waitingQueue_.pop();

        VehicleInfo info;
        info.plate = vehicle->plate;
        info.hour = vehicle->EntryTime.hour;
        snapshot.push_back(info);

        waitingQueue_.push(std::move(vehicle));
    }

    return snapshot;
}

const ExitLog &ParkingLot::getExitLog() const
{
    return exitLog_;
}

ExitLog &ParkingLot::getExitLog()
{
    return exitLog_;
}

void ParkingLot::clearExitLog()
{
    exitLog_.clear();
}

const std::vector<std::unique_ptr<Vehicle>> &ParkingLot::parkedVehicles() const
{
    return parkedVehicles_;
}

std::vector<ExitRecord> ParkingLot::sortedExitRecords() const
{
    return exitLog_.sortedByExitTime();
}

// ---------------------------------------------------------------------------
// Entry
// ---------------------------------------------------------------------------

EnterResult ParkingLot::enterVehicle(const std::string &plate, const DateTime &now)
{
    EnterResult result;
    result.plate = normalize_plate(plate);
    result.hour = now.hour;
    result.remainingSpaces = remainingSpaces();
    result.queueSize = waitingQueue_.size();

    // makeVehicle() normalises the plate and rejects an invalid one.
    Vehicle candidate{};
    if (!makeVehicle(plate, now, candidate))
    {
        result.status = EnterResult::INVALID_PLATE;
        return result;
    }
    result.plate = candidate.plate;

    if (isParked(candidate.plate))
    {
        result.status = EnterResult::ALREADY_PARKED;
        return result;
    }

    if (isWaiting(candidate.plate))
    {
        result.status = EnterResult::ALREADY_QUEUED;
        return result;
    }

    if (remainingSpaces() > 0)
    {
        parkedVehicles_.push_back(std::unique_ptr<Vehicle>(new Vehicle(candidate)));
        result.status = EnterResult::SUCCESS;
    }
    else
    {
        // The joining time is kept so the admin view can show it.
        waitingQueue_.push(std::unique_ptr<Vehicle>(new Vehicle(candidate)));
        result.status = EnterResult::QUEUED;
    }

    result.remainingSpaces = remainingSpaces();
    result.queueSize = waitingQueue_.size();
    return result;
}

// ---------------------------------------------------------------------------
// Exit
// ---------------------------------------------------------------------------

ExitResult ParkingLot::exitVehicle(const std::string &plate, const DateTime &now)
{
    const std::string normalised = normalize_plate(plate);

    ExitResult result;
    result.remainingSpaces = remainingSpaces();

    const std::size_t index = parkedIndex(normalised);
    if (index == parkedVehicles_.size())
    {
        result.status = ExitResult::NOT_PARKED;
        return result;
    }

    const DateTime entryTime = parkedVehicles_[index]->EntryTime;

    const Scenario scenario = calculateDuration(entryTime, now);
    const double fee =
        static_cast<double>(calculateAmount(scenario, 0.0));

    ExitRecord record;
    record.licensePlate = parkedVehicles_[index]->plate;
    record.entryTime = entryTime;
    record.exitTime = now;
    record.fee = fee;

    // The unique_ptr deletes the Vehicle as it leaves the vector.
    parkedVehicles_.erase(parkedVehicles_.begin() +
                          static_cast<std::ptrdiff_t>(index));

    // Push the finished session onto the std::stack that is the exit log.
    exitLog_.push(record);

    const std::vector<std::string> admitted = admitFromQueue(now);

    result.status = ExitResult::SUCCESS;
    result.durationHours = scenario.duration;
    result.fee = fee;
    result.remainingSpaces = remainingSpaces();

    if (!admitted.empty())
    {
        result.promotedFromQueue = true;
        result.promotedPlate = admitted[0];
        result.promotedHour = now.hour;
    }

    return result;
}

std::vector<std::string> ParkingLot::admitFromQueue(const DateTime &now)
{
    std::vector<std::string> admitted;

    while (!waitingQueue_.empty() && remainingSpaces() > 0)
    {
        std::unique_ptr<Vehicle> vehicle = std::move(waitingQueue_.front());
        waitingQueue_.pop();

        // The space was freed at `now`, so that is the new entry time.
        vehicle->EntryTime = now;
        admitted.push_back(vehicle->plate);
        parkedVehicles_.push_back(std::move(vehicle));
    }

    return admitted;
}

// ---------------------------------------------------------------------------
// Cancelling a queued vehicle
// ---------------------------------------------------------------------------

CancelResult ParkingLot::cancelQueue(const std::string &plate)
{
    const std::string normalised = normalize_plate(plate);
    const std::size_t count = waitingQueue_.size();
    bool removed = false;

    for (std::size_t i = 0; i < count; ++i)
    {
        std::unique_ptr<Vehicle> vehicle = std::move(waitingQueue_.front());
        waitingQueue_.pop();

        if (!removed && vehicle->plate == normalised)
        {
            removed = true; // Do not push it back: it is deleted here.
        }
        else
        {
            waitingQueue_.push(std::move(vehicle));
        }
    }

    CancelResult result;
    result.plate = normalised;
    result.queueSize = waitingQueue_.size();
    result.status = removed ? CancelResult::SUCCESS : CancelResult::NOT_IN_QUEUE;
    return result;
}

// ---------------------------------------------------------------------------
// Used by Persistence when loading saved data
// ---------------------------------------------------------------------------

bool ParkingLot::restoreParkedVehicle(const std::string &plate,
                                      const DateTime &entryTime)
{
    Vehicle candidate{};
    if (!makeVehicle(plate, entryTime, candidate))
    {
        return false;
    }
    if (isParked(candidate.plate) || remainingSpaces() <= 0)
    {
        return false;
    }

    parkedVehicles_.push_back(std::unique_ptr<Vehicle>(new Vehicle(candidate)));
    return true;
}

bool ParkingLot::restoreQueuedVehicle(const std::string &plate,
                                      const DateTime &queuedAt)
{
    Vehicle candidate{};
    if (!makeVehicle(plate, queuedAt, candidate))
    {
        return false;
    }
    if (isWaiting(candidate.plate))
    {
        return false;
    }

    waitingQueue_.push(std::unique_ptr<Vehicle>(new Vehicle(candidate)));
    return true;
}

void ParkingLot::restoreExitRecord(const ExitRecord &record)
{
    exitLog_.push(record);
}
