#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "Matrix_9x9.hpp"   //cell, std::map, std::utility
#include <cstdint>          //uint8_t

using namespace std;

class Sudoku {
    private:
        uint8_t display_matrix[27][27];
        Matrix_9x9 mat;

        map<uint8_t, cell> create_map();
        
    public:
        Sudoku();
        ~Sudoku() {}
};

#endif
