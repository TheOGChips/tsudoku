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
        //TODO: Overload bracket operators using a map-and-pair combo
        const uint8_t operator [] (uint8_t);
};

#endif
