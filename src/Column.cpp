#include "../include/Column.hpp"
#include <cstdint>

Column::Column ()
{
    for (uint8_t i = 0; i < 9; i++) {
        col[i] = '-';
    }
}
