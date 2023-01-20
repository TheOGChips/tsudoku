#include "Column.hpp"   //uint8_t

Column::Column ()
{
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, '?');
        //set_value(i, i + 49);
    }
}

Column::Column (uint8_t arr[9])
{
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, arr[i]);
    }
}
