#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "Matrix_9x9.hpp"   //cell, <map>, <utility>
#include <cstdint>  //uint8_t

using namespace std;

Matrix_9x9 sudoku_init();
map<uint8_t, cell> create_map();

#endif
