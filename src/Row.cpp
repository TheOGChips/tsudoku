#include "Row.hpp"
#include <cstdint>

Row::Row ()
{
    for (uint8_t i = 0; i < 9; i++) {
        //row[i] = '-';
        row[i] = i + 49;
    }
}

const uint8_t Row::operator [] (uint8_t index)
{
    return row[index];
}