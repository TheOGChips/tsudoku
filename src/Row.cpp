#include "Row.hpp"
#include <cstdint>

Row::Row ()
{
    for (uint8_t i = 0; i < 9; i++) {
        //row[i] = '-';
        row[i] = i + 49;
    }
}

uint8_t Row::at (uint8_t index) const
{
    return row[index];
}

uint8_t Row::operator [] (uint8_t index) const
{
    //return row[index];
    return at(index);
}