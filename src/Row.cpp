#include "Row.hpp"  //uint8_t

Row::Row ()
{
    for (uint8_t i = 0; i < 9; i++) {
        row[i] = '?';
        //row[i] = i + 49;
    }
}

Row::Row (uint8_t arr[9])
{
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, arr[i]);
    }
}

uint8_t Row::at (uint8_t index)
{
    return row[index];
}

uint8_t Row::operator [] (uint8_t index)
{
    return at(index);
}

bool Row::value_exists (const uint8_t VALUE)
{
    const uint8_t CONVERTED = (VALUE == '?') ? VALUE : VALUE + 48;
    for (uint8_t i = 0; i < 9; i++) {
        if (at(i) == CONVERTED) {
            return true;
        }
    }
    return false;
}

void Row::set_value (uint8_t index, uint8_t value)
{
    row[index] = value;
}

bool Row::evaluate() {
    if (value_exists('?')) return false;
    for (uint8_t i = 1; i <= 9; i++) {
        if (not value_exists(i)) return false;
    }
    return true;
}
