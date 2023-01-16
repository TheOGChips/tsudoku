#ifndef VALUES_HPP
#define VALUES_HPP

#include <cstdint>

#define PURE_VIRTUAL 0

#undef KEY_ENTER
const uint8_t KEY_ENTER = 10,
              ORIGINy = 3,  //NOTE: Sudoku puzzle origin coordinates (top left cell)
              ORIGINx = 6;

enum Values { ONE = '1', TWO, THREE, FOUR, FIVE,
                         SIX, SEVEN, EIGHT, NINE};
#endif
