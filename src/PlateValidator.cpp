#include "PlateValidator.h"
#include <string>
#include <cctype>

/**
 * @brief Check a plate string against this system's plate-format rules.
 *
 * Rejects: empty or longer than MAX_LEN (blanks count), leading/trailing
 * blank, two blanks in a row, "VV", any of I/O/Q, any character other than
 * A-Z, 0-9 and blank, and five or more identical characters in a row.
 * Based on Hong Kong vehicle plates rules
 * @param plate Plate text. Must already be upper case, so call
 *              normalize_plate() first; "ab1" is rejected.
 * @return true if every rule above is satisfied, false otherwise.
 */

bool validate_plate(const std::string &plate){
    int len = plate.size();
    if (len <= MAX_LEN && len > 0){   //length limit
        if (plate[0] == ' ' || plate[len-1] == ' '){   //no leading and ending blanks
            return false;
        }
        for (int i = 0; i < len; i++){  //no characters in ban list
            unsigned char c = static_cast<unsigned char>(plate[i]);
            if (plate[i] == BAN_LIST[0] || plate[i] == BAN_LIST[1] || plate[i] == BAN_LIST[2]
                || !(std::isupper(c) || std::isdigit(c) || c == ' ')) {
                return false;
            }
        }
        char prev = plate[0];
        for (int i = 1; i < len; i++){  //no consecutive blanks and two V.
            char current = plate[i];
            if (current == prev && (prev == ' ' || prev == 'V')){
                return false;
            }
            prev = current;
        }
        if (len >= 4){  //no 4 consecutive identical char or numbers
            int consecutive_count = 1;
            prev = plate[0];
            for (int i = 1; i < len; i++){
                char current = plate[i];
                if (current == prev){
                    consecutive_count ++;
                    if (consecutive_count > 4){
                        return false;
                    }
                }
                else{
                    consecutive_count = 1;
                }
                prev = current;
            }
        }
        return true;
    }
    else{
        return false;
    }
}

/**
* @brief make sure that the plate numbers are all in capital
* Turn every lower case character in the input to upper case.
* e.g. "ab12" -> "AB12", "ABC24" -> "ABC24"
* @param plate Plate text. 
*              e.g. "ab12", "ABC24"
* @return normalized plate number string all in capital
**/

std::string normalize_plate(const std::string &plate){
    std::string s = plate;
    for(char &ch : s){
        ch = static_cast<char>(std::toupper(static_cast<unsigned char> (ch)));
    }
    return s;
}