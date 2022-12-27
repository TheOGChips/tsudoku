#include "Column.hpp"   //uint8_t

Column::Column ()
{
    for (uint8_t i = 0; i < 9; i++) {
        col[i] = '?';
        //col[i] = i + 49;
    }
}

Column::Column (uint8_t arr[9])
{
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, arr[i]);
    }
}

uint8_t Column::at (uint8_t index)
{
    return col[index];
}

uint8_t Column::operator [] (uint8_t index)
{
    return at(index);
}

bool Column::value_exists (const uint8_t VALUE)
{
    const uint8_t CONVERTED = VALUE + 48;
    for (uint8_t i = 0; i < 9; i++) {
        if (at(i) == CONVERTED) {
            return true;
        }
    }
    return false;
}

void Column::set_value (uint8_t index, uint8_t value)
{
    col[index] = value;
}
