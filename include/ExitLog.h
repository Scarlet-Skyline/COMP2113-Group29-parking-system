// ExitLog.h
// COMP2113 Group Project: Parking Lot Management System
//
// The exit log required by the outline: a std::stack<ExitRecord>.
//
// A stack is the natural container because a new record is simply pushed on
// top when a vehicle leaves.  The stack keeps no order that a reader can use
// (it is "last exit first"), so the class also offers sortedByExitTime(),
// which returns a copy of the records sorted from the earliest exit to the
// latest one - the view the administrator actually needs.

#ifndef EXIT_LOG_H
#define EXIT_LOG_H

#include <cstddef>
#include <stack>
#include <vector>

#include "ExitRecord.h"

/**
 * @brief Stack of completed parking sessions.
 */
class ExitLog
{
public:
    /**
     * @brief Add one finished session to the top of the stack.
     */
    void push(const ExitRecord &record);

    /**
     * @brief Remove every record (the "clear the log" admin command).
     */
    void clear();

    bool empty() const;

    std::size_t size() const;

    /**
     * @brief The record on top of the stack, i.e. the most recent exit.
     * @throws std::logic_error if the log is empty.
     */
    const ExitRecord &mostRecent() const;

    /**
     * @brief Copy of the log sorted by exit time, earliest exit first.
     *
     * Uses std::sort (Module 9) with ExitRecord::operator<, which compares
     * exit times and breaks ties by plate.  The stack itself is not changed.
     */
    std::vector<ExitRecord> sortedByExitTime() const;

private:
    std::stack<ExitRecord> records_;
};

#endif // EXIT_LOG_H
