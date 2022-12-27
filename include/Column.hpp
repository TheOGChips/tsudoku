#ifndef COLUMN_HPP
#define COLUMN_HPP

#include "values.hpp"
#include <cstdint>  //uint8_t

class Column {
    private:
        char col[9];

    public:
        Column();
        Column (uint8_t[9]);
        ~Column() {}
        uint8_t at (uint8_t);
        uint8_t operator [] (uint8_t);
        bool value_exists (uint8_t);
        void set_value (uint8_t, uint8_t);
};

#endif
