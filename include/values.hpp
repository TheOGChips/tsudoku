#ifndef VALUES_HPP
#define VALUES_HPP

#include <cstdint>  //uint8_t
#include <utility>  //std::pair
#include <string>

#define PURE_VIRTUAL 0

#undef KEY_ENTER
const uint8_t KEY_ENTER = 10,
              ORIGINy = 3,  //NOTE: Sudoku puzzle origin coordinates (top left cell)
              ORIGINx = 6,
              DISPLAY_MATRIX_SIZE = 27;

enum Values { ONE = '1', TWO, THREE, FOUR, FIVE,
                         SIX, SEVEN, EIGHT, NINE};
                         
typedef std::pair<uint8_t, uint8_t> cell;

const std::string DIR = std::string(getenv("HOME")) + "/.tsudoku";

#endif
