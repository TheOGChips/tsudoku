#include "Column.hpp"
#include <cstdint>

Column::Column ()
{
    for (uint8_t i = 0; i < 9; i++) {
        //col[i] = '-';
        col[i] = i + 49;
    }
}

uint8_t Column::at (uint8_t index) const
{
    return col[index];
}

uint8_t Column::operator [] (uint8_t index) const
{
    //return col[index];
    return at(index);
}
