#ifndef PLATE_VALIDATOR_H
#define PLATE_VALIDATOR_H

#include <string>

int const MAX_LEN = 8;
char const BAN_LIST[3] = {'I', 'O', 'Q'};
//Blanks count as characters, and there can be no more than one blank between any two letters or numerals.
//No more than 4 identical letters or numerals can sit side by side.
//It can't resemble permit or licence numbers, such as "VV".
//The letters "AM", "A", "F", "LC" and "ZG" are reserved for certain vehicles.

bool validate_plate(std::string plate);

#endif