// ParkingLot.h
// COMP2113 Group Project: Parking Lot Management System
//
// Core business class.  It owns
//   * the vehicles inside the car park   std::vector<std::unique_ptr<Vehicle>>
//   * the vehicles waiting for a space   std::queue<std::unique_ptr<Vehicle>>
//   * the log of completed sessions      ExitLog (a std::stack<ExitRecord>)
// and implements entry, exit, queuing, cancelling a place in the queue, the
// remaining-space count and the automatic release of queued vehicles when a
// space becomes free.
//
// Times are DateTime values (TimeUtils.h), matching Vehicle and FeeCalculator.
// The result types below are the interface used by UserMenu.cpp and
// AdminMenu.cpp: every operation returns a status code plus the data the menus
// print, so this class stays free of std::cout and is easy to test.

#ifndef PARKING_LOT_H
#define PARKING_LOT_H

#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "ExitLog.h"
#include "ExitRecord.h"
#include "TimeUtils.h"
#include "Vehicle.h"

/**
 * @brief One row of the administrator's parked / queued table.
 */
struct VehicleInfo
{
    std::string plate; //!< Normalised plate.
    int hour;          //!< Entry hour (parked) or joining hour (queued).
    DateTime when;     //!< Full entry / joining time, used by Persistence.
};

/**
 * @brief Result of a request to let a vehicle enter the car park.
 */
struct EnterResult
{
    enum Status
    {
        SUCCESS,        //!< A space was free; the vehicle is now parked.
        QUEUED,         //!< The car park was full; the vehicle joined the queue.
        ALREADY_PARKED, //!< The plate is already inside the car park.
        ALREADY_QUEUED, //!< The plate is already waiting in the queue.
        INVALID_PLATE   //!< makeVehicle() rejected the plate.
    };

    Status status{SUCCESS};
    std::string plate;        //!< Normalised plate.
    int hour{0};              //!< Entry hour, or the hour the vehicle queued.
    int remainingSpaces{0};   //!< Free spaces after the operation.
    std::size_t queueSize{0}; //!< Vehicles waiting after the operation.
};

/**
 * @brief Result of a request to let a vehicle leave the car park.
 */
struct ExitResult
{
    enum Status
    {
        SUCCESS,   //!< The vehicle left; the fee was charged and logged.
        NOT_PARKED //!< The plate is not inside the car park.
    };

    Status status{NOT_PARKED};
    int durationHours{0};          //!< Charged hours.
    double fee{0.0};               //!< Fee in HKD (FeeCalculator).
    double purchase{0.0};          //!< Purchase amount used for the discounts.
    int remainingSpaces{0};        //!< Free spaces after the operation.
    bool promotedFromQueue{false}; //!< true if a queued vehicle was admitted.
    std::string promotedPlate;     //!< That vehicle's plate.
    int promotedHour{0};           //!< The hour it was admitted.
};

/**
 * @brief Result of a request to give up a place in the queue.
 */
struct CancelResult
{
    enum Status
    {
        SUCCESS,     //!< The plate was removed from the queue.
        NOT_IN_QUEUE //!< The plate is not waiting in the queue.
    };

    Status status{NOT_IN_QUEUE};
    std::string plate;        //!< Normalised plate.
    std::size_t queueSize{0}; //!< Vehicles still waiting.
};

/**
 * @brief The car park itself.
 */
class ParkingLot
{
public:
    /**
     * @brief Create a car park with `capacity` spaces.
     * @throws std::invalid_argument if capacity is not positive.
     */
    explicit ParkingLot(int capacity);

    // The car park owns its vehicles, so it must not be copied.
    ParkingLot(const ParkingLot &) = delete;
    ParkingLot &operator=(const ParkingLot &) = delete;

    /**
     * @brief Try to bring a vehicle in (outline module 2, entry).
     *
     * A vehicle is parked when a space is free and queued when the car park
     * is full.  The plate is normalised and validated by makeVehicle().
     *
     * @param plate Plate as typed by the user.
     * @param now   When the vehicle arrives.
     */
    EnterResult enterVehicle(const std::string &plate, const DateTime &now);

    /**
     * @brief Try to drive a vehicle out (outline module 2, exit).
     *
     * The fee comes from FeeCalculator, the session is pushed onto the exit
     * log, and queued vehicles are admitted while spaces remain.
     *
     * @param plate    Plate as typed by the user.
     * @param now      When the vehicle leaves.
     * @param purchase Amount the driver spent in the mall during the stay.
     *                 FeeCalculator grants free hours for at least 300 HKD on a
     *                 weekday (400 HKD at the weekend).  Defaults to 0.0 (no
     *                 receipt), which is what UserMenu currently passes.
     */
    ExitResult exitVehicle(const std::string &plate, const DateTime &now,
                           double purchase = 0.0);

    /**
     * @brief Remove a vehicle from the waiting queue (outline module 5).
     */
    CancelResult cancelQueue(const std::string &plate);

    int capacity() const;

    /**
     * @brief Number of vehicles currently parked.
     */
    int parkedCount() const;

    /**
     * @brief Free spaces, never negative.
     */
    int remainingSpaces() const;

    /**
     * @brief Number of vehicles waiting in the queue.
     */
    std::size_t queueSize() const;

    bool isParked(const std::string &plate) const;

    bool isWaiting(const std::string &plate) const;

    /**
     * @brief Parked vehicles with their entry hour, for the admin view.
     */
    std::vector<VehicleInfo> getParkedSnapshot() const;

    /**
     * @brief Queued vehicles in queue order, for the admin view.
     */
    std::vector<VehicleInfo> getWaitingSnapshot() const;

    const ExitLog &getExitLog() const;

    ExitLog &getExitLog();

    /**
     * @brief Empty the exit log (administrator command).
     */
    void clearExitLog();

    const std::vector<std::unique_ptr<Vehicle>> &parkedVehicles() const;

    /**
     * @brief Exit log sorted by exit time, earliest exit first.
     */
    std::vector<ExitRecord> sortedExitRecords() const;

    // ---- Used by Persistence when loading saved data -------------------
    bool restoreParkedVehicle(const std::string &plate, const DateTime &entryTime);
    bool restoreQueuedVehicle(const std::string &plate, const DateTime &queuedAt);
    void restoreExitRecord(const ExitRecord &record);

private:
    /**
     * @brief Index of `plate` in parkedVehicles_, or size() if absent.
     *        `plate` must already be normalised.
     */
    std::size_t parkedIndex(const std::string &plate) const;

    /**
     * @brief Fill free spaces with vehicles from the front of the queue.
     * @return The plates that were admitted, in the order they entered.
     */
    std::vector<std::string> admitFromQueue(const DateTime &now);

    int capacity_;
    std::vector<std::unique_ptr<Vehicle>> parkedVehicles_;

    // std::queue has no iterators and std::unique_ptr cannot be copied, so a
    // const query walks the queue by rotating it (every element is moved out
    // and pushed back, which restores the exact order).  "mutable" lets that
    // rotation happen inside const member functions such as
    // getWaitingSnapshot(); the queue is logically unchanged afterwards.
    mutable std::queue<std::unique_ptr<Vehicle>> waitingQueue_;

    ExitLog exitLog_;
};

#endif // PARKING_LOT_H
