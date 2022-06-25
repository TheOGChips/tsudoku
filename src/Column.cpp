#include "../include/Column.hpp"

Column::Column ()
{
    for (uint8_t i = 0; i < 9; i++) {
        col[i] = '-';
    }
}
