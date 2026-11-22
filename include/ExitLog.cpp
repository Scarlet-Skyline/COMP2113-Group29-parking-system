// ExitLog.cpp
// COMP2113 Group Project: Parking Lot Management System

#include "ExitLog.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

void ExitLog::push(const ExitRecord &record)
{
    records_.push(record);
}

void ExitLog::clear()
{
    // std::stack has no clear(), so swap it with an empty stack.
    std::stack<ExitRecord> empty;
    std::swap(records_, empty);
}

bool ExitLog::empty() const
{
    return records_.empty();
}

std::size_t ExitLog::size() const
{
    return records_.size();
}

const ExitRecord &ExitLog::mostRecent() const
{
    if (records_.empty())
    {
        throw std::logic_error("ExitLog::mostRecent(): the exit log is empty");
    }
    return records_.top();
}

std::vector<ExitRecord> ExitLog::sortedByExitTime() const
{
    // A std::stack cannot be iterated over, so copy it and pop the copy.
    std::vector<ExitRecord> records;
    records.reserve(records_.size());

    std::stack<ExitRecord> copy = records_;
    while (!copy.empty())
    {
        records.push_back(copy.top());
        copy.pop();
    }

    // ExitRecord::operator< orders by exit time (earliest first).
    std::sort(records.begin(), records.end());

    return records;
}
