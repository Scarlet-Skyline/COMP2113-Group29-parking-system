#include "PlateValidator.h"
#include <string>
#include <cctype>

bool validate_plate(std::string plate){
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
