#include "Container.hpp"

Container::Container () {
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, '?');
        //set_value(i, i + 49);
    }
}

Container::Container (uint8_t arr[9]) {
    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, arr[i]);
    }
}

uint8_t Container::at (uint8_t index) {
    return arr[index];
}

uint8_t Container::operator [] (uint8_t index) {
    return at(index);
}

bool Container::value_exists (const uint8_t VALUE) {
    const uint8_t CONVERTED = (VALUE == '?') ? VALUE : VALUE + 48;
    for (uint8_t i = 0; i < 9; i++) {
        if (at(i) == CONVERTED) {
            return true;
        }
    }
    return false;
}

void Container::set_value (uint8_t index, uint8_t value) {
    arr[index] = value;
}

bool Container::evaluate() {
    if (value_exists('?')) return false;
    for (uint8_t i = 1; i <= 9; i++) {
        if (not value_exists(i)) return false;
    }
    return true;
}
