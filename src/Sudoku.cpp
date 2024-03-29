#include <ncurses.h>
#include "Sudoku.hpp"
#include "colors.hpp"
#include <cctype>
#include <thread>
#include <chrono>
#include <fstream>
#include "InGameMenu.hpp"
#include <filesystem>
#include "DifficultyMenu.hpp"
#include <csignal>

using namespace std;

/* NOTE:
 * Name: Class Constructor (overloaded)
 * Purpose: Coordinates setup of color mappings and display matrix initialization.
 * Parameters:
 *      SAVED_PUZZLE -> Pointer to a SavedPuzzle object that represents a previously saved game. If
 *                      the user has selected to start a new game, this will be a nullptr. If the
 *                      user has selected to resume a saved game, this object will be read in
 *                      beforehand.
 */
Sudoku::Sudoku (const SavedPuzzle* SAVED_PUZZLE) {
    signal(SIGINT, SIGINT_handler);
    create_map();
    set_color_pairs();  //NOTE: Establish color pairs for display matrix
    init_display_matrix(SAVED_PUZZLE);
}

/* NOTE:
 * Name: create_map
 * Purpose: Creates a mapping between the 81 cells in a grid to their positions in the display
 *          matrix. A reverse mapping is also created simultaneously. This mapping assumes a display
 *          matrix origin of (0, 0), and a offset is applied later during actual display.
 * Parameters: None
 */
void Sudoku::create_map () {
    uint8_t row = 1,
            column = 1;

    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        grid2display_map[i] = cell(row, column);
        display2grid_map[cell(row, column)] = i;
        column += 3;
        if (column / DISPLAY_MATRIX_COLUMNS) {
            column %= DISPLAY_MATRIX_COLUMNS;
            row += 3;
        }
    }
}

/* NOTE:
 * Name: map_display_matrix_offset
 * Purpose: Creates a mapping between a cell in the display matrix and it's actual location on the
 *          screen. A call to this function is made for one cell at a time during the initial
 *          printing of the display matrix to the screen.
 * Parameters:
 *      DISPLAY_INDECES -> Cell object containing the display line and display column number.
 * 
 * NOTE: This looks like it doesn't work as expected, but the use of the overloaded Sudoku::move in
 *       printw takes care of applying the offset before this function is called.
 */
void Sudoku::map_display_matrix_offset (const cell DISPLAY_INDECES) {
    uint8_t y,
            x;
    getyx(stdscr, y, x);
    cell coords = {y, x};
    display_matrix_offset[coords] = DISPLAY_INDECES;
}

/* NOTE:
 * Name: set_color_pairs
 * Purpose: Establishes the color pairs used while printing anywhere in the display matrix. The
 *          color pair MENU_SELECTION is defined inside MainMenu.cpp, and its value is carried over
 *          throughout the rest of the program. In the case coloring is not available (in the event
 *          this somehow finds its way onto some old machine), a monochrome mode is also provided
 *          where everything is the same color.
 * Parameters: None
 */
void Sudoku::set_color_pairs () { 
    /* NOTE: I'm guessing this should work like this, but I don't have a non-color-supported
     *       terminal to test this out on, and this is the simplest thing to do without adding
     *       checks everywhere. If someone else knows or finds that this function doesn't work as
     *       intended, feel free to correct it.
     */
    if (has_colors()) { //NOTE: Color mode
        init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
        init_pair(GIVEN, COLOR_RED, COLOR_BLACK);
        init_pair(CANDIDATES_Y, COLOR_YELLOW, COLOR_BLACK);
        init_pair(CANDIDATES_B, COLOR_BLUE, COLOR_BLACK);
        init_pair(GUESS, COLOR_GREEN, COLOR_BLACK);
    }
    else {  //NOTE: Monochrome mode
        init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
        init_pair(GIVEN, COLOR_BLACK, COLOR_WHITE); //NOTE: Reversed to better stand out
        init_pair(CANDIDATES_Y, COLOR_WHITE, COLOR_BLACK);
        init_pair(CANDIDATES_B, COLOR_WHITE, COLOR_BLACK);
        init_pair(GUESS, COLOR_WHITE, COLOR_BLACK);
    }
}

/* NOTE:
 * Name: init_display_matrix
 * Purpose: Initialiizes the display matrix with either a newly generated puzzle or a saved game.
 * Parameters:
 *      SAVED_PUZZLE -> Pointer to a SavedPuzzle object that represents a previously saved game. If
 *                      the user has selected to start a new game, this will be a nullptr. If the
 *                      user has selected to resume a saved game, this object will be read in
 *                      beforehand.
 */
void Sudoku::init_display_matrix(const SavedPuzzle* SAVED_PUZZLE) {
    /* NOTE: This is a display matrix indeces "cheat sheet", with Grid cells mapped out. This will
     *       display as intended if looking at it full screen with 1920x1080 screen dimensions.
     * 
     *   0,0  0,1  0,2  0,3  0,4  0,5  0,6  0,7  0,8 |  0,9  0,10  0,11  0,12  0,13  0,14  0,15  0,16  0,17 |  0,18  0,19  0,20  0,21  0,22  0,23  0,24  0,25  0,26
     *   1,0  1,1            1,4            1,7      |       1,10              1,13              1,16       |        1,19              1,22              1,25
     *   2,0                                         |                                                      |
     *   3,0                                         |                                                      |
     *   4,0  4,1            4,4            4,7      |       4,10              4,13              4,16       |        4,19              4,22              4,25
     *   5,0                                         |                                                      |
     *   6,0                                         |                                                      |
     *   7,0  7,1            7,4            7,7      |       7,10              7,13              7,16       |        7,19              7,22              7,25
     *   8,0                                         |                                                      |
     *  ---------------------------------------------|------------------------------------------------------|------------------------------------------------------
     *   9,0                                         |                                                      |
     *  10,0 10,1           10,4           10,7      |      10,10             10,13             10,16       |       10,19             10,22             10,25
     *  11,0                                         |                                                      |
     *  12,0                                         |                                                      |
     *  13,0 13,1           13,4           13,7      |      13,10             13,13             13,16       |       13,19             13,22             13,25
     *  14,0                                         |                                                      |
     *  15,0                                         |                                                      |
     *  16,0 16,1           16,4           16,7      |      16,10             16,13             16,16       |       16,19             16,22             16,25
     *  17,0                                         |                                                      |
     *  ---------------------------------------------|------------------------------------------------------|------------------------------------------------------
     *  18,0                                         |                                                      |
     *  19,0 19,1           19,4           19,7      |      19,10             19,13             19,16       |       19,19             19,22             19,25
     *  20,0                                         |                                                      |
     *  21,0                                         |                                                      |
     *  22,0 22,1           22,4           22,7      |      22,10             22,13             22,16       |       22,19             22,22             22,25
     *  23,0                                         |                                                      |
     *  24,0                                         |                                                      |
     *  25,0 25,1           25,4           25,7      |      25,10             25,13             25,16       |       25,19             25,22             25,25
     *  26,0                                         |                                                      |
     */

    //NOTE: Initialize display matrix with blank spaces
    if (not SAVED_PUZZLE) {
        for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
            for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
                display_matrix[i][j] = ' ';
            }
        }
        DifficultyMenu diff_menu;
        diff_menu.menu();
        
        this->grid = Grid(diff_menu.get_difficulty_level());
        /* NOTE: This call to create_map() actually works as originally intended, but the
         *       constructor call is currently used because of the catch-22 that occurs when
         *       resuming a game.
         */
        //create_map();
        for (uint8_t i = 0; i < grid2display_map.size(); i++) {
            cell coords = grid2display_map[i];
            display_matrix[coords.first][coords.second] = this->grid[i];
        }
    }
    else {
        for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
            for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
                display_matrix[i][j] = SAVED_PUZZLE->puzzle[i][j];
            }
        }
        
        uint8_t grid[NUM_CONTAINERS][NUM_CONTAINERS] = {};
        for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
            for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
                cell coords = grid2display_map[i*CONTAINER_SIZE + j];
                grid[i][j] = SAVED_PUZZLE->puzzle[coords.first][coords.second];
            }
        }
        
        #if DEBUG
            clear();
            mvprintw(0, 0, "Printing grid...");
            for (uint8_t i = 0; i < NUM_CONTAINERS; i++) {
                for (uint8_t j = 0; j < NUM_CONTAINERS; j++) {
                    mvprintw(i + 1, j, "%c", grid[i][j]);
                    
                }
            }
            refresh();
            getch();
        #endif
        this->grid = Grid(grid);
        /* NOTE: This call to create_map() currently fails if run here. There's a sort of catch-22
         *       where (to work as intended) the grid passed to grid needs grid2display_map to be
         *       filled, but grid2display_map also needs grid to be filled.
         */
        //create_map();
    }
    
    #if DEBUG
        ::printw("Printing mapping...\n");
        for (uint8_t i = 0; i < this->grid.get_map_size(); i++) {
            ::printw("m[%u]: (%u, %u)", i, this->grid.get_position(i).first,
                     this->grid.get_position(i).second);
            (i+1) % NUM_CONTAINERS ? ::printw("\t") : ::printw("\n");
        }
        ::printw("\n\n");
        for (uint8_t i = 0; i < grid2display_map.size(); i++) {
            ::printw("m[%u]: (%u, %u)", i, grid2display_map[i].first, grid2display_map[i].second);
            (i+1) % NUM_CONTAINERS ? ::printw("\t") : ::printw("\n");
        }
        ::printw("\n\n");
        for (uint8_t i = 0; i < display2grid_map.size(); i++) {
            ::printw("rm[(%u, %u)]:\t%u", grid2display_map[i].first, grid2display_map[i].second,
                     display2grid_map[grid2display_map[i]]);
            (i+1) % NUM_CONTAINERS ? ::printw("\t") : ::printw("\n");
        }
        refresh();
        getch();
        clear();
    #endif
    
    #if DEBUG
        enum print_by {row, column, box};
        for (uint8_t i = row; i <= box; i++) {
            ::printw("Printing by ");
            if (i == box) {
                ::printw("box...\n");
            }
            else if (i == column) {
                ::printw("column...\n");
            }
            else {
                ::printw("row...\n");
            }
            this->grid.printw(i & column, i & box);
            refresh();
            getch();
            clear();
        }
    #endif
}

/* NOTE:
 * Name: printw (NCurses library function overload)
 * Purpose: Prints the entire sudoku puzzle (the display matrix) to the screen for initial viewing.
 * Parameters:
 *      SAVED_PUZZLE -> Pointer to a SavedPuzzle object that represents a previously saved game. If
 *                      the user has selected to start a new game, this will be a nullptr. If the
 *                      user has selected to resume a saved game, this object will be read in
 *                      beforehand.
 */
void Sudoku::printw (const SavedPuzzle* SAVED_PUZZLE) {
    #if DEBUG
        ::printw("Printing display matrix...\n");
    #endif
    
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        move(cell {i, 0}); //NOTE: Call to Sudoku::move wrapper function (applies display offset)
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            map_display_matrix_offset(cell {i, j});
            
            uint8_t color_pair;
            if (SAVED_PUZZLE) {
                switch (SAVED_PUZZLE->color_codes[i][j]) {
                    case 'u': color_pair = UNKNOWN;
                              break;
                              
                    case 'r': color_pair = GIVEN;
                              break;
                              
                    case 'y': color_pair = CANDIDATES_Y;
                              attron(A_BOLD);
                              break;
                              
                    case 'b': color_pair = CANDIDATES_B;
                              attron(A_BOLD);
                              break;
                              
                    case 'g': color_pair = GUESS;
                              break;
                              
                    default: color_pair = 0;    //NOTE: case 'n'
                }
                
                color_codes[i][j] = color_pair;
                attron(COLOR_PAIR(color_pair));
            }
            ::printw("%c", display_matrix[i][j]);
            if (SAVED_PUZZLE) {
                attroff(COLOR_PAIR(color_pair));
                attroff(A_BOLD);
            }
            
            if (j == 8 or j == 17) ::printw("|");
        }
        if (i == 8 or i == 17) {
            ::move(i + ORIGIN.first + (i / CONTAINER_SIZE) + 1, ORIGIN.second);
            ::printw("---------|---------|---------");
        }
    }
    
    if (not SAVED_PUZZLE) {
        for (uint8_t i = 0; i < grid2display_map.size(); i++) {
            cell coords = grid2display_map[i];
            move(coords);
            
            if (grid.is_known(i)) {
                color_codes[coords.first][coords.second] = GIVEN;
                attron(COLOR_PAIR(GIVEN));
                ::printw("%c", display_matrix[coords.first][coords.second]);
                attroff(COLOR_PAIR(GIVEN));
            }
            else {
                color_codes[coords.first][coords.second] = UNKNOWN;
                attron(COLOR_PAIR(UNKNOWN));
                ::printw("%c", display_matrix[coords.first][coords.second]);
                attroff(COLOR_PAIR(UNKNOWN));
            }
        }
    }
}

/* NOTE:
 * Name: printw (NCurses library function overload)
 * Purpose: Prints the entire sudoku puzzle (the display matrix) to the screen whenever there has
 *          been an update by the player (i.e. removal or insertion of a value).
 * Parameters: None
 */
void Sudoku::printw () {
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        move(cell {i, 0}); //NOTE: Call to Sudoku::move wrapper function (applies display offset)
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            if (color_codes[i][j] == CANDIDATES_Y or
                color_codes[i][j] == CANDIDATES_B) attron(A_BOLD);
            attron(COLOR_PAIR(color_codes[i][j]));
            ::printw("%c", display_matrix[i][j]);
            attroff(COLOR_PAIR(color_codes[i][j]));
            attroff(A_BOLD);
            
            if (j == 8 or j == 17) ::printw("|");
        }
        if (i == 8 or i == 17) {
            ::move(i + ORIGIN.first + (i / CONTAINER_SIZE) + 1, ORIGIN.second);
            ::printw("---------|---------|---------");
        }
    }
}

/* NOTE:
 * Name: move (NCurses library function overload #1)
 * Purpose: Moves the cursor to its offset position for the initial printing of the display matrix
 *          from Sudoku::printw. This is necessary so that the the display matrix offset can be
 *          mapped correctly. This has the same parameter prototype as NCurses's printw.
 * Parameters:
 *      COORDS -> Pre-offset display line and column numbers.
 */
void Sudoku::move (const cell COORDS) {
    const uint8_t TOTAL_OFFSETY = COORDS.first + ORIGIN.first  + (COORDS.first / CONTAINER_SIZE),
                  TOTAL_OFFSETX = COORDS.second + ORIGIN.second + (COORDS.second / CONTAINER_SIZE);

    ::move(TOTAL_OFFSETY, TOTAL_OFFSETX);
    getyx(stdscr, cursor_pos.first, cursor_pos.second); //NOTE: Update cursor_pos after moving
}

/* NOTE:
 * Name: move (NCurses library function overload #2)
 * Purpose: Moves the cursor around the display matrix based on the user's input key. This variant
 *          is called during interactive play after the display matrix has already been rendered to
 *          the screen. All other inputs other than an arrow key are ignored.
 * Parameters:
 *      KEY -> An integer corresponding to a key pressed by the user. The mappings for keys are
 *             handled by NCurses.
 */
void Sudoku::move (const uint16_t KEY) {
    static const uint8_t MAX_YBOUNDARY = ORIGIN.first + DISPLAY_MATRIX_ROWS + 1,
                         MAX_XBOUNDARY = ORIGIN.second + DISPLAY_MATRIX_COLUMNS + 1;

    switch (KEY) {
        case 's':
        case KEY_DOWN:
            if (cursor_pos.first < MAX_YBOUNDARY) {
                if (is_box_border(cell {cursor_pos.first + 1, cursor_pos.second})) {
                    ::move(cursor_pos.first + 2, cursor_pos.second);
                }
                else ::move(cursor_pos.first + 1, cursor_pos.second);
                getyx(stdscr, cursor_pos.first, cursor_pos.second);
            }
            break;
            
        case 'w':
        case KEY_UP:
            if (cursor_pos.first > ORIGIN.first) {
                if (is_box_border(cell {cursor_pos.first - 1, cursor_pos.second})) {
                    ::move(cursor_pos.first - 2, cursor_pos.second);
                }
                else ::move(cursor_pos.first - 1, cursor_pos.second);
                    getyx(stdscr, cursor_pos.first, cursor_pos.second);
            }
            break;
                
        case 'a':
        case KEY_LEFT:
            if (cursor_pos.second > ORIGIN.second) {
                if (is_box_border(cell {cursor_pos.first, cursor_pos.second - 1})) {
                    ::move(cursor_pos.first, cursor_pos.second - 2);
                }
                else ::move(cursor_pos.first, cursor_pos.second - 1);
                getyx(stdscr, cursor_pos.first, cursor_pos.second);
            }
            break;
            
        case 'd':
        case KEY_RIGHT:
            if (cursor_pos.second < MAX_XBOUNDARY) {
                if (is_box_border(cell {cursor_pos.first, cursor_pos.second + 1})) {
                    ::move(cursor_pos.first, cursor_pos.second + 2);
                }
                else ::move(cursor_pos.first, cursor_pos.second + 1);
                getyx(stdscr, cursor_pos.first, cursor_pos.second);
            }
            break;
        default:;   /* NOTE: This shouldn't ever be run because the of the switch statement in
                     *       start_game().
                     */
    }
}

/* NOTE:
 * Name: refresh (NCurses library function overload)
 * Purpose: Updates the terminal display with any changes. This is a wrapper around the NCurses
 *          function of the same name.
 * Parameters: None
 */
void Sudoku::refresh () {
    ::refresh();
}

/* NOTE:
 * Name: getch (NCurses library function overload)
 * Purpose: Returns the character at the current cursor position. This is a wrapper around the
 *          NCurses function of the same name. Since NCurses getch is really a macro, it has been
 *          undefined in misc.hpp in order for this function to be defined here. The underlying call
 *          to NCurses wgetch(stdscr) seen here is the same functionality as the original NCurses
 *          getch.
 * Parameters: None
 */
uint16_t Sudoku::getch () {
    return ::wgetch(stdscr);
}

/* NOTE:
 * Name: clear (NCurses library function overload)
 * Purpose: Clears the terminal of all output. This is a wrapper around the NCurses function of the
 *          same name.
 * Parameters: None
 */
void Sudoku::clear () {
    ::clear();
}

/* NOTE:
 * Name: is_box_border
 * Purpose: Determines whether a cell contains a box border character. This is what enables the
 *          cursor to skip over such a cell when moving. Returns true if the character is either a
 *          '|' or '-'; false otherwise.
 * Parameters:
 *      COORDS -> Line and column numbers of the cell to be checked.
 */
bool Sudoku::is_box_border (const cell COORDS) {
    chtype ch = mvinch(COORDS.first, COORDS.second);
    return ((ch == '|') | (ch == '-'));
}

/* NOTE:
 * Name: get_surrounding_cells
 * Purpose: Returns an array containing the surrounding the cell objects representing the cells
 *          surrounding the cursor's current position.
 * Parameters: None
 */
array<cell, Sudoku::NUM_BORDER_POSITIONS> Sudoku::get_surrounding_cells () {
    /* NOTE: This has to be done this way, or else the array initialization doesn't work for some
     *       weird reason.
     */
    cell tl = {cursor_pos.first - 1, cursor_pos.second - 1};
    cell t  = {cursor_pos.first - 1, cursor_pos.second};
    cell tr = {cursor_pos.first - 1, cursor_pos.second + 1};
    cell l  = {cursor_pos.first,     cursor_pos.second - 1};
    cell r  = {cursor_pos.first,     cursor_pos.second + 1};
    cell bl = {cursor_pos.first + 1, cursor_pos.second - 1};
    cell b  = {cursor_pos.first + 1, cursor_pos.second};
    cell br = {cursor_pos.first + 1, cursor_pos.second + 1};
    array<cell, NUM_BORDER_POSITIONS> border = {tl, t, tr, l, r, bl, b, br};

    return border;
}

/* NOTE:
 * Name: do_nothing
 * Purpose: Determines whether no action should be taken based on the cursor's current position.
 *          Returns true if the cursor's position or any of the 8 surrounding cells contain a given
 *          number; false otherwise.
 * Parameters: None
 */
bool Sudoku::do_nothing () {
    //NOTE: Get the 8 cells around the current cursor position
    array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
    
    //NOTE: Check all surrounding cells for given number
    for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
        if ((mvinch(border[i].first, border[i].second) & A_COLOR) == COLOR_PAIR(GIVEN)) return true;
    }
    reset_cursor();
    
    //NOTE: Lastly, check if cursor's current position contains a given number.
    return (inch() & A_COLOR) == COLOR_PAIR(GIVEN);
}

/* NOTE:
 * Name: clear_surrounding_cells
 * Purpose: Clears the surrounding cells of the cursor's position of their values. This is only done
 *          when entering a number into a guess cell, but not when removing.
 * Parameters: None
 */
void Sudoku::clear_surrounding_cells () {
    //NOTE: Get the 8 cells around the current cursor position
    array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
    for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
        mvprintw(border[i].first, border[i].second, " ");
        const uint8_t Y = display_matrix_offset[border[i]].first,
                      X = display_matrix_offset[border[i]].second;
        display_matrix[Y][X] = ' ';
    }
}

/* NOTE:
 * Name: set_value
 * Purpose: Places or removes a value in the display matrix with the appropriate coloring if the
 *          cursor's current position is a valid cell for input. The appropriate Row, Column, and
 *          Box from the internal Grid member is updated with the value if the cursor's position is
 *          over a guess cell (i.e. a cell that is also mapped by the Grid's own internal map).
 * Parameters:
 *      VALUE -> The value to be placed into the display matrix and (possibly) the appropriate Row,
 *               Column, and Box of this game's Grid member variable. If the value corresponds to
 *               that of the Delete or Backspace keys, this function performs a removal instead.
 * 
 * NOTE: This now uses the new overload of Sudoku::printw (although the original lines have been
 *       left as comments for posterity), but there's some issue with how the color pairs for
 *       candidate cells are determined when I try to do a similar simplification in the original
 *       Sudoku::printw (the one with parameters). The display2grid_map always produces a 0, which
 *       means all the candidate cells become yellow. I've made several attempts to see if I could
 *       figure out why this might be, but I don't think it's worth taking the time to "fix" when it
 *       already works perfectly fine.
 */
void Sudoku::set_value (const uint16_t VALUE) {
    /* NOTE: Algorithm for determining where and/or how to place a value entered by the user
     * 
     * if value is red (starting value)
     *      ignore, do nothing
     * if position is not mapped to position in 9x9 matrix
     *      place value in display matrix only
     *      display value on screen
     *      refresh
     * if position is mapped to position in 9x9 matrix
     *      place value in display matrix
     *      clear 8 surrounding cells
     *      refresh
     *      place into appropriate spot in appropriate row, column, and box
     */
    
    if (do_nothing()) reset_cursor();
    else {
        const uint8_t Y = display_matrix_offset[cursor_pos].first,
                      X = display_matrix_offset[cursor_pos].second;
                
        reset_cursor();
        chtype ch = inch();
        if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN) or (ch & A_COLOR) == COLOR_PAIR(GUESS)) {
            const uint8_t INDEX = display2grid_map[display_matrix_offset[cursor_pos]];
            
            if (VALUE == KEY_DC or VALUE == KEY_BACKSPACE) {
                if ((ch & A_COLOR) == COLOR_PAIR(GUESS)) {
                    /*attron(COLOR_PAIR(UNKNOWN));
                    mvprintw(cursor_pos.first, cursor_pos.second, "?");
                    attroff(COLOR_PAIR(UNKNOWN));*/
                    
                    grid.set_value(INDEX, '?');
                    display_matrix[Y][X] = '?';
                    color_codes[Y][X] = UNKNOWN;
                }
                //else if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN)) {}    //Do nothing
            }
            else {
                clear_surrounding_cells();
                /*attron(COLOR_PAIR(GUESS));
                mvprintw(cursor_pos.first, cursor_pos.second, "%c", VALUE);
                attroff(COLOR_PAIR(GUESS));*/
                
                grid.set_value(INDEX, VALUE);
                display_matrix[Y][X] = VALUE;
                color_codes[Y][X] = GUESS;
                
                #if DEBUG
                    ::mvprintw(25, 40 + 20, "index: %d", INDEX);
                    ::mvprintw(26, 40 + 20, "row #: %d", grid.map_row(INDEX));
                    ::mvprintw(27, 40 + 20, "col #: %d", grid.map_column(INDEX));
                #endif
            }

            #if DEBUG
                enum print_by {row, column, box};
                for (uint8_t i = row; i <= box; i++) {
                    ::move(9, 40 + 20 * i);
                    if (i == box) {
                        ::printw("box");
                    }
                    else if (i == column) {
                        ::printw("column");
                    }
                    else {
                        ::printw("row");
                    }
                    grid.mvprintw(cell {10, 40 + 20 * i}, i & column, i & box);
                    refresh();
                }

                refresh();
            #endif
        }
        else {
            if (VALUE == KEY_DC or VALUE == KEY_BACKSPACE) {
                //::printw(" ");
                display_matrix[Y][X] = ' ';
                color_codes[Y][X] = 0;
            }
            else {
                array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
                uint8_t color_pair;
                for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
                    chtype ch = mvinch(border[i].first, border[i].second);
                    if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN) or
                        (ch & A_COLOR) == COLOR_PAIR(GUESS)) {
                        color_pair = (display2grid_map[display_matrix_offset[border[i]]] % 2) ?
                                      CANDIDATES_B : CANDIDATES_Y;
                    }
                }
                reset_cursor();
                
                /*attron(COLOR_PAIR(color_pair));
                attron(A_BOLD);
                ::printw("%c", VALUE);
                attroff(A_BOLD);
                attroff(COLOR_PAIR(color_pair));*/
                
                display_matrix[Y][X] = VALUE;
                color_codes[Y][X] = color_pair;
            }
        }
        refresh();
    }
    
    cell curr_pos = cursor_pos;
    printw();
    cursor_pos = curr_pos;
    reset_cursor(); //NOTE: Have cursor maintain position after printing
}

/* NOTE:
 * Name: reset_cursor
 * Purpose: Resets the cursor to its last officially recorded position. This is mainly used after
 *          needing to temporarily move to another cell to read or remove a value and gives the
 *          appearance that the cursor never moved at all.
 * Parameters: None
 */
void Sudoku::reset_cursor () {
    ::move(cursor_pos.first, cursor_pos.second);
}

/* NOTE:
 * Name: evaluate
 * Purpose: Calls the Grid member to evaluate its Rows, Columns, and Boxes for validity (i.e. a
 *          valid solution or solved puzzle). Returns true only if the puzzle currently has a valid
 *          solution.
 * Parameters: None
 */
bool Sudoku::evaluate () {
    return grid.evaluate();
}

/* NOTE:
 * Name: increment_completed_games
 * Purpose: Increments the number of completed games recorded by 1. This is only called once the
 *          user has solved the current puzzle. Ensuring file pathname validity (whether the file
 *          and its directory exist) is handled elsewhere at the start of the program before the
 *          main menu appears.
 * Parameters: None
 */
void Sudoku::increment_completed_games () {
    string HOME = getenv("HOME"),
           filename = HOME + "/.tsudoku/completed_puzzles.txt";
    uint64_t num_completed;
    
    ifstream infile;
    infile.open(filename.c_str());
    infile >> num_completed;
    infile.close();
    
    ofstream outfile;
    outfile.open(filename.c_str());
    outfile << ++num_completed << endl;
    outfile.close();
}

/* NOTE:
 * Name: save_game
 * Purpose: Saves the current state of the puzzle to a file. The name of the file is chosen by the
 *          user, but is restricted to 15 characters. Actual saving is handled by a call to
 *          InGameMenu's static save_game function. This function just provides compatibility for
 *          when the in-game menu is disabled.
 * Parameters: None
 */
void Sudoku::save_game (const uint8_t DELAY) {
    const uint8_t DISPLAY_LINE = ORIGIN.first + DISPLAY_MATRIX_ROWS + 3;
    
    ::move(DISPLAY_LINE, 1);
    clrtoeol();
    ::printw("Enter save file name: ");
    
    //NOTE: Copy the display matrix into a pointer in order to pass along to InGameMenu's save_game.
    uint8_t* display_matrix[DISPLAY_MATRIX_COLUMNS];
    for (uint8_t i = 0; i < DISPLAY_MATRIX_COLUMNS; i++) {
        display_matrix[i] = this->display_matrix[i];
    }
    
    //NOTE: Display confirmation that the game has been saved.
    const string NAME = InGameMenu::save_game(display_matrix);
    ::move(DISPLAY_LINE, 1);
    clrtoeol();
    curs_set(false);    //NOTE: Turn off cursor while displaying
    if (not NAME.empty()) mvprintw(DISPLAY_LINE, ORIGIN.second, "%s saved!", NAME.c_str());
    else mvprintw(DISPLAY_LINE, ORIGIN.second, "Error: File not saved!");
    refresh();
    
    //NOTE: Clear output after a delay
    this_thread::sleep_for(chrono::seconds(DELAY));
    ::move(DISPLAY_LINE, 0);
    clrtoeol();
    curs_set(true); //NOTE: Turn cursor back on before returning to the game
}

/* NOTE:
 * Name: display_hotkey
 * Purpose: Displays the hotkey command available in the bottom left corner depending on whether the
 *          in-game menu is enabled.
 * Parameters:
 *      USE_IN_GAME_MENU -> Boolean controlling whether or not the in-game menu is enabled. This is
 *                          determined based on whether or not the user runs this program with the
 *                          "--no-in-game-menu" or "-n" command line options. This also controls
 *                          which hotkey is available.
 *      LINE_OFFSET_TWEAK -> Line offset from max line number used to display hotkey command in an
 *                           ideal location.
 */
void Sudoku::display_hotkey (const bool USE_IN_GAME_MENU, const uint8_t LINE_OFFSET_TWEAK) {
    if (not USE_IN_GAME_MENU) {
        attron(COLOR_PAIR(MENU_SELECTION));
        ::mvprintw(getmaxy(stdscr) - LINE_OFFSET_TWEAK, ORIGIN.second, "s -> save game");
        attroff(COLOR_PAIR(MENU_SELECTION));
    }
    else {
        attron(COLOR_PAIR(MENU_SELECTION));
        ::mvprintw(getmaxy(stdscr) - LINE_OFFSET_TWEAK, ORIGIN.second, "m -> in-game menu");
        attroff(COLOR_PAIR(MENU_SELECTION));
    }
}

/* NOTE:
 * Name: start_game
 * Purpose: Starts and runs a game of sudoku until the user either wins or decides to quit.
 *          Dispatches calls to the in-game menu (when enabled), to directly save the game (when the
 *          in-game menu isn't enabled), to exit, to move the cursor, or to handle input values for
 *          the display matrix and Grid member. This function also handles removing the saved game
 *          file and updating the number of games completed if the user solves the current puzzle.
 * Parameters:
 *      USE_IN_GAME_MENU -> Boolean controlling whether or not the in-game menu is enabled. This is
 *                          determined based on whether or not the user runs this program with the
 *                          "--no-in-game-menu" or "-n" command line options.
 *      SAVED_PUZZLE -> Pointer to a SavedPuzzle object that represents a previously saved game. If
 *                      the user has selected to start a new game, this will be a nullptr. If the
 *                      user has selected to resume a saved game, this object will be read in
 *                      beforehand.
 */
void Sudoku::start_game (const bool USE_IN_GAME_MENU, const SavedPuzzle* SAVED_PUZZLE) {
    printw(SAVED_PUZZLE);
    const uint8_t LINE_OFFSET_TWEAK = 3,    //NOTE: # lines to get display output correct
                  DELAY = 2;                //NOTE: # seconds to delay after printing out results
                  
    display_hotkey(USE_IN_GAME_MENU, LINE_OFFSET_TWEAK);
    ::move(ORIGIN.first, ORIGIN.second);    //NOTE: starting position of the user
    cursor_pos = make_pair(ORIGIN.first, ORIGIN.second);
    refresh();

    bool quit_game = false;
    //nodelay(stdscr, true);
    timeout(250);
    do {
        int16_t input = getch();    //NOTE: Signed needed because getch can return ERR=-1 on timeout
        if (tolower(input) == 'q') {    //NOTE: This check has to be here first for this
            quit_game = true;           //      to work as expected. Not sure why.
        }
        else if (tolower(input) == 'm' and USE_IN_GAME_MENU) {
            //NOTE: Allows re-using the same in-game menu object on each loop iteration
            static InGameMenu in_game_menu(display_matrix); 
            
            //NOTE: Toggle hotkey meaning while in in-game menu
            attron(COLOR_PAIR(MENU_SELECTION));
            mvprintw(getmaxy(stdscr) - LINE_OFFSET_TWEAK, ORIGIN.second, "m -> return to game");
            attroff(COLOR_PAIR(MENU_SELECTION));
            clrtoeol();
            
            in_game_menu.menu();
            cell saved_pos = cursor_pos;                        //NOTE: Save cursor position before
            if (in_game_menu.get_window_resized()) printw();    //      (potentially) needing to
                                                                //      reprint the puzzle
            //NOTE: Toggle hotkey back to original meaning when leaving in-game menu
            attron(COLOR_PAIR(MENU_SELECTION));
            mvprintw(getmaxy(stdscr) - LINE_OFFSET_TWEAK, ORIGIN.second, "m -> in-game menu");
            attroff(COLOR_PAIR(MENU_SELECTION));
            clrtoeol();
            
            refresh();
            cursor_pos = saved_pos; //NOTE: Restore cursor position before resetting
            reset_cursor();
        }
        else if (tolower(input) == 's' and not USE_IN_GAME_MENU) {
            save_game(DELAY);
            reset_cursor();
        }
        else if ((input >= KEY_DOWN and input <= KEY_RIGHT) or
                 input == 'a' or input == 's' or input == 'd' or input == 'w') {
                     move(input);
        }
        else if (input >= ONE and input <= NINE)            set_value(input);
        else if (input == KEY_DC or input == KEY_BACKSPACE) set_value(input);
        else if (input == KEY_ENTER) {                          
            curs_set(false);
            ::mvprintw(ORIGIN.first + DISPLAY_MATRIX_ROWS + LINE_OFFSET_TWEAK, ORIGIN.second,
                       "Result: ");
            if (evaluate()) {
                string msg = "You win!";
                ::printw("%s", msg.c_str());
                clrtoeol();
                refresh();
                increment_completed_games();
                quit_game = true;
                this_thread::sleep_for(chrono::seconds(DELAY));
                
                if (SAVED_PUZZLE) filesystem::remove(SAVED_PUZZLE->filename);
            }
            else {
                string msg = "Puzzle incomplete!";
                ::printw("%s", msg.c_str());
                refresh();
                this_thread::sleep_for(chrono::seconds(DELAY));
                ::move(ORIGIN.first + DISPLAY_MATRIX_ROWS + LINE_OFFSET_TWEAK, 0);
                clrtoeol();
                reset_cursor();
            }
            curs_set(true);
        }
        else {
            if (invalid_window_size_handler()) {
                cell curr_pos = cursor_pos;
                printw();
                cursor_pos = curr_pos;
                display_hotkey(USE_IN_GAME_MENU, LINE_OFFSET_TWEAK);
                reset_cursor();
            }
        }
    } while (!quit_game);
    nodelay(stdscr, false);
}

/* NOTE:
 * Name: SIGINT_handler
 * Purpose: Resets the terminal settings to their previous state from before the NCurses environment
 *          was initialized.
 * Parameters:
 *      (unused 32-bit integer) -> The signal being caught by this handler, in this case SIGINT.
 *                                 Since it's value is never used, it does not need a name for
 *                                 reference.
 */
void Sudoku::SIGINT_handler (int32_t) {
    curs_set(true);
    echo();
    nocbreak();
    endwin();
    exit(EXIT_SUCCESS);
}
