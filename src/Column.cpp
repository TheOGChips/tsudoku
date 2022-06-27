#include "Column.hpp"
#include <cstdint>

Column::Column ()
{
    for (uint8_t i = 0; i < 9; i++) {
        //col[i] = '-';
        col[i] = i + 49;
    }
}

const uint8_t Column::operator [] (uint8_t index)
{
    return col[index];
}
