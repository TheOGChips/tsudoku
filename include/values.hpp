#ifndef VALUES_HPP
#define VALUES_HPP

#include <cstdint>  //uint8_t
#include <utility>  //std::pair

#define PURE_VIRTUAL 0

#undef KEY_ENTER
const uint8_t KEY_ENTER = 10,
              ORIGINy = 3,  //NOTE: Sudoku puzzle origin coordinates (top left cell)
              ORIGINx = 6;

enum Values { ONE = '1', TWO, THREE, FOUR, FIVE,
                         SIX, SEVEN, EIGHT, NINE};
                         
typedef std::pair<uint8_t, uint8_t> cell;

#endif
