// Persistence.h
// File I/O for the parking lot state.
//
// Plain text, one record per line, first character is the kind:
//   P <plate> <y> <m> <d> <H> <M>          parked vehicle + entry time
//   Q <plate> <y> <m> <d> <H> <M>          queued vehicle + joining time
//   E <plate> <ey> <em> <ed> <eh> <emi>
//             <xy> <xm> <xd> <xh> <xmi> <fee>
//                                          one exit record
//
// Loading uses the existing ParkingLot::restore* functions, so the
// validation rules (plate format, capacity, duplicates) are still applied.

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <string>

#include "ParkingLot.h"

namespace Persistence
{
    //! Save the state of `lot` to `path`. Returns false on I/O failure.
    bool save(const ParkingLot &lot, const std::string &path);

    //! Load `path` into `lot`. Missing file is treated as "no data" (false).
    bool load(ParkingLot &lot, const std::string &path);
}

#endif // PERSISTENCE_H