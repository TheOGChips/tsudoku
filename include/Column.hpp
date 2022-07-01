#ifndef COLUMN_HPP
#define COLUMN_HPP

#include "values.hpp"
#include <cstdint>

class Column {
    private:
        char col[9];

    public:
        Column();
        ~Column() {}
        uint8_t at (uint8_t) const;
        uint8_t operator [] (uint8_t) const;
};

#endif
