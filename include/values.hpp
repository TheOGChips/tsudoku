#ifndef VALUES_HPP
#define VALUES_HPP

//TODO: Rename this to something like "misc.hpp" or "misc_values.hpp"

#include <cstdint>  //uint8_t
#include <utility>  //std::pair
#include <string>

#define PURE_VIRTUAL 0

#undef KEY_ENTER
const uint8_t KEY_ENTER = 10,
              ORIGINy = 3,  //NOTE: Sudoku puzzle origin coordinates (top left cell)
              ORIGINx = 6,
              DISPLAY_MATRIX_ROWS = 27,
              DISPLAY_MATRIX_COLUMNS = DISPLAY_MATRIX_ROWS,
              CONTAINER_SIZE = 9,
              NUM_CONTAINERS = 9;

enum Values { ONE = '1', TWO, THREE, FOUR, FIVE,
                         SIX, SEVEN, EIGHT, NINE};
                         
typedef std::pair<uint8_t, uint8_t> cell;

const std::string DIR = std::string(getenv("HOME")) + "/.tsudoku";

struct SavedPuzzle {
    uint8_t puzzle[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS];
    char color_codes[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS];
    std::string filename;
};

enum class difficulty_level { EASY = 60, MEDIUM = 45, HARD = 30, EXPERT = 17 };

#endif
