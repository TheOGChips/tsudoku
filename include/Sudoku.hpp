#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "Grid.hpp" //cell, std::map, std::utility
#include <cstdint>  //uint8_t
#include <array>    //std::array

#undef getch    /* NOTE:
                 * Redefined as Sudoku::getch(). This doesn't appear to affect the getch() calls in
                 * main.cpp, even though it logically should after seeing how rearranging #include
                 * statements got the KEY_ENTER redefinition to work.
                 */
using namespace std;

/* NOTE:
 * Class: Sudoku
 * Purpose: Represents an interactive live game of sudoku.
 * Private variables:
 *      display_matrix -> 27x27 matrix of 8-bit characters that is displayed to the screen during
 *                        play. This is the data structure the user directly interacts with.
 *      grid -> Grid object representing just the 81 cells of a sudoku board.
 *      grid2display_map -> Mapping of 81 positions of a grid to their (y, x) coordinates in the
 *                          display matrix.
 *      display2grid_map -> Reverse mapping of _map_.
 *      ORIGIN -> Starting cell of the display matrix's (0, 0) position on the actual terminal
 *                window. This is effectively the row and column offset from the top left cell of the
 *                terminal and also controls the size of the buffers from the edges of the terminal
 *                window.
 *                NOTE: Terminal coordinates are in (y,x) format. Origin coordinates can be found in
 *                      misc.hpp
 *      cursor_pos -> The current position of the cursor on the display matrix. At the start of the
 *                    game, this is equivalent to the ORIGIN, and his is constantly tracked and
 *                    updated every time the cursor moves afterwards. This is primarily beneficial
 *                    for resetting (with the appearance of maintaining) the cursor on the current
 *                    position after reacting to user input (i.e. this object's primary purpose is to
 *                    make the cursor appear as if it never moved after the user inputs a character
 *                    on the display matrix).
 *      display_matrix_offset -> Mapping of the display matrix's index pairs to their (y, x)
 *                               positions displayed in the terminal window. This makes the math
 *                               behind making position-related changes easier. display_matrix[0][0]
 *                               is mapped to the ORIGIN and so forth.
 *      neighbor_cells -> Enumeration of shorthand constants used when retrieving a cell's 8
 *                          surrounding (i.e. border) cells along with a constant for the number of
 *                          border positions.
 */
class Sudoku {
    private:
        uint8_t display_matrix[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS] = {};
        uint8_t color_codes[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS] = {};
        Grid grid;
        map<uint8_t, cell> grid2display_map;
        map<cell, uint8_t> display2grid_map;
        const cell ORIGIN = { ORIGINy, ORIGINx };
        cell cursor_pos;
        map<cell, cell> display_matrix_offset;
        enum neighbor_cells { TL = 0, T, TR, L, R, BL, B, BR, NUM_BORDER_POSITIONS };

        void create_map ();
        void map_display_matrix_offset (const cell);
        void set_color_pairs ();
        void init_display_matrix (const SavedPuzzle*);
        void printw (const SavedPuzzle*);
        void printw ();
        void move (const cell);
        void move (const uint16_t);
        void refresh ();
        uint16_t getch ();
        void clear ();
        bool is_box_border (const cell);
        array<cell, NUM_BORDER_POSITIONS> get_surrounding_cells ();
        bool do_nothing ();
        void clear_surrounding_cells ();
        void set_value (const uint16_t);
        void reset_cursor ();
        bool evaluate ();
        void increment_completed_games ();
        void save_game (const uint8_t);
        void display_hotkey (const bool, const uint8_t);
        
    public:
        Sudoku (const SavedPuzzle*);
        ~Sudoku () {}

        void start_game (const bool, const SavedPuzzle*);
        static void SIGINT_handler (int32_t);
};

#endif
