// Persistence.cpp

#include "Persistence.h"

#include <ctime>
#include <fstream>
#include <sstream>
#include <string>

#include "TimeUtils.h"

namespace
{

// Convert a std::time_t to the broken-down DateTime used by the rest of the
// program.  localtime_s / localtime_r make this thread-safe.
DateTime timeTToDateTime(std::time_t t)
{
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    DateTime d{};
    d.year   = tmv.tm_year + 1900;
    d.month  = tmv.tm_mon + 1;
    d.day    = tmv.tm_mday;
    d.hour   = tmv.tm_hour;
    d.minute = tmv.tm_min;
    return d;
}

} // namespace

namespace Persistence
{

bool save(const ParkingLot &lot, const std::string &path)
{
    std::ofstream out(path);
    if (!out)
    {
        return false;
    }

    for (const VehicleInfo &v : lot.getParkedSnapshot())
    {
        out << "P " << v.plate << ' '
            << v.when.year   << ' ' << v.when.month  << ' ' << v.when.day   << ' '
            << v.when.hour   << ' ' << v.when.minute << '\n';
    }

    for (const VehicleInfo &v : lot.getWaitingSnapshot())
    {
        out << "Q " << v.plate << ' '
            << v.when.year   << ' ' << v.when.month  << ' ' << v.when.day   << ' '
            << v.when.hour   << ' ' << v.when.minute << '\n';
    }

    // sortedExitRecords() walks the stack and returns earliest-first, which
    // is exactly what we want to write out.
    for (const ExitRecord &r : lot.sortedExitRecords())
    {
        const DateTime e = timeTToDateTime(r.entryTime);
        const DateTime x = timeTToDateTime(r.exitTime);
        out << "E " << r.licensePlate << ' '
            << e.year << ' ' << e.month << ' ' << e.day << ' ' << e.hour << ' ' << e.minute << ' '
            << x.year << ' ' << x.month << ' ' << x.day << ' ' << x.hour << ' ' << x.minute << ' '
            << r.fee << '\n';
    }

    return static_cast<bool>(out);
}

bool load(ParkingLot &lot, const std::string &path)
{
    std::ifstream in(path);
    if (!in)
    {
        return false; // no file -> nothing to load, not an error
    }

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);
        char kind = '\0';
        iss >> kind;

        if (kind == 'P')
        {
            std::string plate;
            DateTime d{};
            if (iss >> plate
                    >> d.year >> d.month >> d.day >> d.hour >> d.minute)
            {
                lot.restoreParkedVehicle(plate, d);
            }
        }
        else if (kind == 'Q')
        {
            std::string plate;
            DateTime d{};
            if (iss >> plate
                    >> d.year >> d.month >> d.day >> d.hour >> d.minute)
            {
                lot.restoreQueuedVehicle(plate, d);
            }
        }
        else if (kind == 'E')
        {
            std::string plate;
            DateTime e{}, x{};
            double fee = 0.0;
            if (iss >> plate
                    >> e.year >> e.month >> e.day >> e.hour >> e.minute
                    >> x.year >> x.month >> x.day >> x.hour >> x.minute
                    >> fee)
            {
                ExitRecord r;
                r.licensePlate = plate;
                r.entryTime    = toTimeT(e);
                r.exitTime     = toTimeT(x);
                r.fee          = fee;
                lot.restoreExitRecord(r);
            }
        }
        // Any other first character is silently ignored.
    }

    return true;
}

} // namespace Persistence