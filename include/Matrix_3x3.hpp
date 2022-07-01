#ifndef MATRIX_3X3_HPP
#define MATRIX_3X3_HPP

#include "values.hpp"
#include <map>
#include <utility>  //pair
#include <cstdint>

using namespace std;

typedef pair<uint8_t, uint8_t> cell;

class Matrix_3x3 {
    private:
        char mat[3][3];
        map<uint8_t, cell> m;
        map<uint8_t, cell> create_map();

    public:
        Matrix_3x3 ();
        ~Matrix_3x3 () {}
        const uint8_t at (uint8_t);
        const uint8_t operator [] (uint8_t);
        void print_map();
};

#endif
