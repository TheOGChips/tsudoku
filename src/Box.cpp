#include "Box.hpp"  //uint8_t

Box::Box ()
{
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, '?');
        //set_value(i, i + 49);
    }
}

Box::Box (uint8_t arr[9])
{
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, arr[i]);
    }
}
