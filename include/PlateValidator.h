// PlateValidator.h
// COMP2113 Group Project: Parking Lot Management System
//
// Plate format checking.  Text typed by a user goes through normalize_plate()
// (fixes the letter case) and then validate_plate() (checks the format rules).
// makeVehicle() in Vehicle.h performs both steps.

#ifndef PLATE_VALIDATOR_H
#define PLATE_VALIDATOR_H

#include <string>

int const MAX_LEN = 8;                    //!< Longest allowed plate in characters (blanks count).
char const BAN_LIST[3] = {'I', 'O', 'Q'}; //!< Letters that never appear on a plate.

/**
 * @brief Check a plate against this system's plate-format rules.
 *
 * A plate is rejected if any of the following holds:
 *  - it is empty or longer than MAX_LEN characters (blanks count);
 *  - it starts or ends with a blank, or has two blanks in a row;
 *  - it contains a character other than A-Z, 0-9 and blank, or a letter
 *    from BAN_LIST (I, O, Q);
 *  - it contains "VV" (too close to a permit or licence number);
 *  - the same character appears five or more times in a row.
 *
 * Not checked: the reserved prefixes AM, A, F, LC and ZG.
 *
 * @param plate Plate text, already in upper case.  Call normalize_plate()
 *              first, because lower-case input such as "ab1" is rejected.
 * @return true if no rule above is broken, false otherwise.
 */
bool validate_plate(const std::string &plate);

/**
 * @brief Return a copy of the plate with every letter in upper case.
 *
 * Only the letter case changes.  Blanks are kept, nothing is trimmed and the
 * result is not validated.
 *
 * @param plate Plate text as typed by the user.
 * @return The upper-case copy.
 */
std::string normalize_plate(const std::string &plate);

#endif