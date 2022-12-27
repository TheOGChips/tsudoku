#ifndef MATRIX_3X3_HPP
#define MATRIX_3X3_HPP

#include "values.hpp"
#include <map>      //std::map
#include <utility>  //std::pair
#include <cstdint>  //uint8_t
#include <array>

using namespace std;

typedef pair<uint8_t, uint8_t> cell;

class Matrix_3x3 {
    private:
        char mat[3][3];
        map<uint8_t, cell> _map_;
        map<uint8_t, cell> create_map();

    public:
        Matrix_3x3 ();
        Matrix_3x3 (uint8_t[9]);
        ~Matrix_3x3 () {}
        uint8_t at (uint8_t);
        uint8_t operator [] (uint8_t);
        void print_map();
        bool value_exists (uint8_t);
        void set_value (uint8_t, uint8_t);
};

#endif
