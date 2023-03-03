#include "Container.hpp"

/* NOTE:
 * Name: Class Constructor (default)
 * Purpose: Initializes internal array to all '?'.
 * Parameters: None
 */
Container::Container () {
    for (uint8_t i = 0; i < CONTAINER_SIZE; i++) {
        set_value(i, '?');
    }
}

/* NOTE:
 * Name: Class Constructor
 * Purpose: Initializes internal array to the same values as it's array parameter.
 * Parameters:
 *      arr -> Integer array of Sudoku numbers or ?s.
 */
Container::Container (const uint8_t ARR[CONTAINER_SIZE]) {
    for (uint8_t i = 0; i < CONTAINER_SIZE; i++) {
        set_value(i, ARR[i]);
    }
}

/* NOTE:
 * Name: at
 * Purpose: Retrieves a value from the internal array.
 * Parameters:
 *      index -> Integer index of internal array that is returned.
 */
uint8_t Container::at (const uint8_t INDEX) const {
    return arr[INDEX];
}

/* NOTE:
 * Name: Operator [] (overloaded)
 * Purpose: Retrieves a value from the internal array.
 * Parameters:
 *      INDEX -> Integer index of internal array that is returned.
 */
uint8_t Container::operator [] (const uint8_t INDEX) {
    return at(INDEX);
}

/* NOTE:
 * Name: value_exists
 * Purpose: Checks whether a value 1-9 already exists in the internal array.
 * Parameters:
 *      VALUE -> Integer value that is searched for in the container's internal array.
 */
bool Container::value_exists (const uint8_t VALUE) const {
    const uint8_t CONVERTED = (VALUE == '?') ? VALUE : VALUE + ZERO;
    for (uint8_t i = 0; i < CONTAINER_SIZE; i++) {
        if (at(i) == CONVERTED) {
            return true;
        }
    }
    return false;
}

/* NOTE:
 * Name: set_value
 * Purpose: Sets an element of the internal array to a specfic value 1-9.
 * Parameters:
 *      INDEX -> Integer index of the internal array
 *      VALUE -> Integer value to be placed into the internal array
 */
void Container::set_value (const uint8_t INDEX, const uint8_t VALUE) {
    arr[INDEX] = VALUE;
}

/* NOTE:
 * Name: evaluate
 * Purpose: Evaluates whether the container's internal array values are valid for a solved sudoku
 *          puzzle (i.e. exactly one each of the values 1-9 in the array).
 * Parameters: None
 */
bool Container::evaluate() const {
    if (value_exists('?')) return false;
    for (uint8_t i = 1; i <= CONTAINER_SIZE; i++) {
        if (not value_exists(i)) return false;
    }
    return true;
}
