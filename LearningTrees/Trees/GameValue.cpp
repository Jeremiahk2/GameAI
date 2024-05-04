#include "GameValue.h"
//Return 0 if first equals second.
//Return -1 if first less than second.
//Return 1 if first greater than second.
//Return -2 if no comparison can be made due to typing, or for false.
int GameValue::compare(const GameValue first, const GameValue second) {
    //First and second must equal, and neither can be NONE
    if (first.type == second.type && first.type != NONE) {
        //Integer number.
        if (first.type == NUMBER) {
            if (*first.data.number < *second.data.number) {
                return -1;
            }
            else if (*first.data.number > *second.data.number) {
                return 1;
            }
            else {
                return 0;
            }
        }
        //Time value (int64_t)
        else if (first.type == TIME) {
            if (*first.data.time < *second.data.time) {
                return -1;
            }
            else if (*first.data.time > *second.data.time) {
                return 1;
            }
            else {
                return 0;
            }
        }
        //Real number (float)
        else if (first.type == REAL) {
            if (*first.data.real < *second.data.real) {
                return -1;
            }
            else if (*first.data.real > *second.data.real) {
                return 1;
            }
            else {
                return 0;
            }
        }
        //Boolean value (bool)
        else if (first.type == BOOLEAN) {
            if (*first.data.boolean) {
                return 0;
            }
            else {
                return -2;
            }
        }
        //None of the above, invalid.
        else {
            return -2;
        }
    }
    //Invalid combination, invalid.
    else {
        return -2;
    }
}