#ifndef VALUES_HPP
#define VALUES_HPP

#include <cstdint>  //uint8_t, uint16_t
#include <utility>  //std::pair
#include <string>   //std::string

// NOTE: This file contains miscellaneous values and data structures needed across some or all files.

#define DEBUG false

#define PURE_VIRTUAL 0  //NOTE: A more helpful note when declaring a pure virtual function. Gives an
                        //      error if used as a regular constant instead of a macro

#undef KEY_ENTER                //NOTE: Redefines the value of the Enter key because the pre-defined 
const uint8_t KEY_ENTER = 10,   //      NCurses value doesn't seem to work.
              ORIGINy = 3,  //NOTE: Sudoku puzzle display origin coordinates (top left cell)
              ORIGINx = 6,
              DISPLAY_MATRIX_ROWS = 27,
              DISPLAY_MATRIX_COLUMNS = DISPLAY_MATRIX_ROWS,
              CONTAINER_SIZE = 9,
              NUM_CONTAINERS = 9;
              
//NOTE: A more helpful way of using the character versions of the 10 digits
enum Values { ZERO = '0', ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE };
                         
typedef std::pair<uint8_t, uint8_t> cell;   //NOTE: A cell in the NCurses terminal display

//NOTE: Hidden directory for storing game data.
const std::string DIR = std::string(getenv("HOME")) + "/.tsudoku";

//NOTE: Wrapper struct for handling saved game data
struct SavedPuzzle {
    uint8_t puzzle[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS];
    char color_codes[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS];
    std::string filename;
};

/* NOTE: Difficulty levels corresponding to the number of grid cells given. According to
 *       https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that
 *       need to be filled in in order to create a uniquely solvable puzzle is 17.
 */
enum class difficulty_level { EASY = 60, MEDIUM = 45, HARD = 30, EXPERT = 17 };

#endif
