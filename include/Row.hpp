#ifndef ROW_HPP
#define ROW_HPP

#include "values.hpp"
#include <cstdint>

class Row {
    private:
        char row[9];

    public:
        Row();
        ~Row() {}
        const uint8_t operator [] (uint8_t);
};

#endif
