#pragma once
#include <memory>
struct GameValue {
    enum {NUMBER, TIME, REAL, BOOLEAN, NONE} type;
    union StateData {
        int *number;
        int64_t *time;
        float *real;
        bool *boolean;
    } data;

    static int compare(GameValue first, GameValue second);
};