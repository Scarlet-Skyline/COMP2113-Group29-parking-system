// ExitRecord.h
// COMP2113 Group Project: Parking Lot Management System
//
// One record of the exit log: who left, when they came in, when they left,
// and how much they paid.  This is a plain data structure (a struct, as
// introduced in Module 7) so it can be copied into and out of the
// std::stack<ExitRecord> that stores the log.

#ifndef EXIT_RECORD_H
#define EXIT_RECORD_H

#include <ctime>
#include <string>

/**
 * @brief A completed parking session.
 *
 * The administrator log and the exit receipt both need the same four pieces
 * of information, so they are kept together in one record.
 */
struct ExitRecord
{
    std::string licensePlate; //!< Plate in normalised form, e.g. "AB1234".
    std::time_t entryTime{0}; //!< When the vehicle entered the car park.
    std::time_t exitTime{0};  //!< When the vehicle left the car park.
    double fee{0.0};          //!< Fee charged, in HKD.

    /**
     * @brief Time spent inside the car park, in seconds.
     *        A clock change that makes the difference negative gives 0.
     */
    long long parkedSeconds() const
    {
        const double difference = std::difftime(exitTime, entryTime);
        return difference > 0.0 ? static_cast<long long>(difference) : 0;
    }

    /**
     * @brief Ordering used to sort the log: by exit time, earliest first.
     *        Records that share an exit time are ordered by plate so that
     *        the result is always the same for the same input.
     */
    bool operator<(const ExitRecord &other) const
    {
        if (exitTime != other.exitTime)
        {
            return exitTime < other.exitTime;
        }
        return licensePlate < other.licensePlate;
    }
};

#endif // EXIT_RECORD_H
