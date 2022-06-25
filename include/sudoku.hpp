#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "Matrix_9x9.hpp"
#include <map>
#include <utility>  //pair
#include <cstdint>  //uint8_t

using namespace std;

typedef pair<uint8_t, uint8_t> cell;

Matrix_9x9 sudoku_init();
map<uint8_t, cell> create_map();

#endif
