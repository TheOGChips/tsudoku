#ifndef ROW_HPP
#define ROW_HPP

#include "values.hpp"
#include <cstdint>  //uint8_t

class Row {
    private:
        char row[9];

    public:
        Row();
        Row (uint8_t[9]);
        ~Row() {}
        uint8_t at (uint8_t);
        uint8_t operator [] (uint8_t);
        bool value_exists (uint8_t);
        void set_value (uint8_t, uint8_t);
        bool evaluate();
};

#endif
