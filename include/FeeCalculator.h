// FeeCalculator.h
// COMP2113 Group Project: Parking Lot Management System
//
// Parking fee rules, in two steps: calculateDuration() turns an entry time and
// an exit time into a Scenario, then calculateAmount() turns the Scenario and
// the customer's purchase into a fee.  The rates, purchase thresholds and free
// hours are the named constants in Constants.h.

#ifndef FEECALCULATOR_H
#define FEECALCULATOR_H

#include "TimeUtils.h"
#include "Constants.h"

/**
 * @brief The facts about one stay that decide its fee.
 */
struct Scenario {
    int duration;   //!< Billable hours, rounded up.  Never negative when made by calculateDuration().
    bool isWeekday; //!< true if the vehicle entered Monday to Friday, false for Saturday or Sunday.
    bool isEvening; //!< true if the vehicle entered at 17:00 or later.
};

/**
 * @brief Work out the billable hours and the day type of a stay.
 *
 * The duration is rounded UP to whole hours (3 h 10 min counts as 4 h).  If
 * the exit is not after the entry, the duration is 0.
 *
 * The weekday/weekend and evening flags come from the ENTRY time only, so a
 * stay keeps the rate of the moment it started, even if it runs past 17:00 or
 * past midnight.  Public holidays are not considered.
 *
 * The dates are not validated: std::mktime rolls an impossible date such as
 * 31/02 forward to a real one.
 *
 * @param entrance_time When the vehicle entered (local time).
 * @param exit_time     When the vehicle left (local time).
 * @return The Scenario to pass to calculateAmount().
 */
Scenario calculateDuration(DateTime entrance_time, DateTime exit_time);

/**
 * @brief Compute the parking fee for one stay.
 *
 * Without a qualifying purchase:
 *     fee = duration x hourly rate
 * With a qualifying purchase the free hours are deducted first, and the fee
 * never goes below 0:
 *     fee = max(0, duration - free hours) x hourly rate
 *
 * The rules per day type (the purchase threshold is inclusive, i.e. >=):
 *  - Weekday: rate FeePerHour.  A purchase of at least MINIMUM_PURCHASE_WEEKDAY
 *    gives WEEKDAY_FREE_HOURS free hours, or WEEKDAY_FREE_HOURS_EVENING if the
 *    stay started in the evening.
 *  - Weekend: rate FeePerHourWeekend.  A purchase of at least
 *    MINIMUM_PURCHASE_WEEKEND gives WEEKEND_FREE_HOURS free hours.  isEvening
 *    has no effect.
 *
 * Precondition: s.duration >= 0, which calculateDuration() guarantees.  A
 * negative duration is not handled and would give a negative fee.
 *
 * @param s        The stay, from calculateDuration().
 * @param purchase Amount the customer spent, in the same currency as the
 *                 MINIMUM_PURCHASE_* thresholds.  Pass 0 for no purchase.
 * @return The fee in HKD, in whole dollars.
 */
int calculateAmount(Scenario s, double purchase);

#endif