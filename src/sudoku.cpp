#include <ncurses.h>
#include "sudoku.hpp"
#include "colors.hpp"
#include <cctype>
#include <thread>
#include <chrono>
#include <fstream>
#include "InGameMenu.hpp"
#include <filesystem>
#include "DifficultyMenu.hpp"

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
    //TODO: Move this note to the location where the difficulty setting is chosen
    /* NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the
     *       minimum amount of tiles that need to be filled in in order to create a uniquely
     *       solvable puzzle is 17 (this will later be HARD difficulty if diffuculty settings are
     *       added later)
     */
    create_map();
    set_color_pairs();  //NOTE: Establish color pairs for display matrix
    //TODO: Will need to account for signal handling
    init_display_matrix(SAVED_PUZZLE);
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::create_map () {
    uint8_t row = 1,
            column = 1;

    for (uint8_t i = 0; i < GRID_SIZE; i++) {
        _map_[i] = cell(row, column);
        _rev_map_[cell(row, column)] = i;
        column += 3;
        if (column / DISPLAY_MATRIX_COLUMNS) {
            column %= DISPLAY_MATRIX_COLUMNS;
            row += 3;
        }
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::map_display_matrix_offset (const uint8_t YINDEX, const uint8_t XINDEX) {
    uint8_t y,
            x;
    getyx(stdscr, y, x);
    cell display_indeces = {YINDEX, XINDEX},
         coords = {y, x};
    display_matrix_offset[coords] = display_indeces;
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::set_color_pairs() { 
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
        init_pair(GIVEN, COLOR_WHITE, COLOR_BLACK);
        init_pair(CANDIDATES_Y, COLOR_WHITE, COLOR_BLACK);
        init_pair(CANDIDATES_B, COLOR_WHITE, COLOR_BLACK);
        init_pair(GUESS, COLOR_WHITE, COLOR_BLACK);
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
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
        //TODO: Retrieve difficulty level and send to Grid somehow
        DifficultyMenu diff_menu;
        diff_menu.menu();
        
        mat = Grid(diff_menu.get_difficulty_level());
        /* NOTE: This call to create_map() actually works as originally intended, but the
         *       constructor call is currently used because of the catch-22 that occurs when
         *       resuming a game.
         */
        //create_map();
        for (uint8_t i = 0; i < _map_.size(); i++) {
            cell coords = _map_[i];
            display_matrix[coords.first][coords.second] = mat[i];
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
                cell coords = _map_[i*CONTAINER_SIZE + j];
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
        mat = Grid(grid);
        /* NOTE: This call to create_map() currently fails if run here. There's a sort of catch-22
         *       where (to work as intended) the grid passed to mat needs _map_ to be filled, but
         *       _map_ also needs mat to be filled.
         */
        //create_map();
    }
    
    #if DEBUG
        ::printw("Printing mapping...\n");
        for (uint8_t i = 0; i < mat.get_map_size(); i++) {
            ::printw("m[%u]: (%u, %u)", i, mat.get_position(i).first, mat.get_position(i).second);
            (i+1) % NUM_CONTAINERS ? ::printw("\t") : ::printw("\n");
        }
        ::printw("\n\n");
        for (uint8_t i = 0; i < _map_.size(); i++) {
            ::printw("m[%u]: (%u, %u)", i, _map_[i].first, _map_[i].second);
            (i+1) % NUM_CONTAINERS ? ::printw("\t") : ::printw("\n");
        }
        ::printw("\n\n");
        for (uint8_t i = 0; i < _rev_map_.size(); i++) {
            ::printw("rm[(%u, %u)]:\t%u", _map_[i].first, _map_[i].second, _rev_map_[_map_[i]]);
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
            mat.printw(i & column, i & box);
            refresh();
            getch();
            clear();
        }
    #endif
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::printw (const SavedPuzzle* SAVED_PUZZLE) {
    #if DEBUG
        ::printw("Printing display matrix...\n");
    #endif
    
    for (uint8_t i = 0; i < DISPLAY_MATRIX_ROWS; i++) {
        move(i, 0);
        for (uint8_t j = 0; j < DISPLAY_MATRIX_COLUMNS; j++) {
            map_display_matrix_offset(i, j);
            
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
                
                attron(COLOR_PAIR(color_pair));
            }
            ::printw("%c", display_matrix[i][j]);
            if (SAVED_PUZZLE) {
                attroff(COLOR_PAIR(color_pair));
                attroff(A_BOLD);
            }
            
            if (j == 8 or j == 17) {
                ::printw("|");
            }
        }
        if (i == 8 or i == 17) {
            ::move(i + ORIGIN.first + (i / CONTAINER_SIZE) + 1, ORIGIN.second);
            ::printw("---------|---------|---------");
        }
    }
    
    if (not SAVED_PUZZLE) {
        for (uint8_t i = 0; i < _map_.size(); i++) {
            cell coords = _map_[i];
            move(coords.first, coords.second);
            
            if (mat.is_known(i)) {
                attron(COLOR_PAIR(GIVEN));
                ::printw("%c", display_matrix[coords.first][coords.second]);
                attroff(COLOR_PAIR(GIVEN));
            }
            else {
                attron(COLOR_PAIR(UNKNOWN));
                ::printw("%c", display_matrix[coords.first][coords.second]);
                attroff(COLOR_PAIR(UNKNOWN));
            }
        }
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::move (const uint8_t YCOORD, const uint8_t XCOORD) {
    const uint8_t TOTAL_OFFSETY = YCOORD + ORIGIN.first  + (YCOORD / CONTAINER_SIZE),
                  TOTAL_OFFSETX = XCOORD + ORIGIN.second + (XCOORD / CONTAINER_SIZE);

    ::move(TOTAL_OFFSETY, TOTAL_OFFSETX);
    getyx(stdscr, cursor_pos.first, cursor_pos.second); //NOTE: Update cursor_pos after moving
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::move (const uint16_t KEY) {
    static const uint8_t MAX_YBOUNDARY = ORIGIN.first + DISPLAY_MATRIX_ROWS + 1,
                         MAX_XBOUNDARY = ORIGIN.second + DISPLAY_MATRIX_COLUMNS + 1;

    switch (KEY) {
        case KEY_DOWN:
            if (cursor_pos.first < MAX_YBOUNDARY) {
                if (is_border(cursor_pos.first + 1, cursor_pos.second)) {
                    ::move(cursor_pos.first + 2, cursor_pos.second);
                }
                else ::move(cursor_pos.first + 1, cursor_pos.second);
                getyx(stdscr, cursor_pos.first, cursor_pos.second);
            }
            break;
        case KEY_UP:
            if (cursor_pos.first > ORIGIN.first) {
                if (is_border(cursor_pos.first - 1, cursor_pos.second)) {
                    ::move(cursor_pos.first - 2, cursor_pos.second);
                }
                else ::move(cursor_pos.first - 1, cursor_pos.second);
                    getyx(stdscr, cursor_pos.first, cursor_pos.second);
                }
                break;
        case KEY_LEFT:
            if (cursor_pos.second > ORIGIN.second) {
                if (is_border(cursor_pos.first, cursor_pos.second - 1)) {
                    ::move(cursor_pos.first, cursor_pos.second - 2);
                }
                else ::move(cursor_pos.first, cursor_pos.second - 1);
                getyx(stdscr, cursor_pos.first, cursor_pos.second);
            }
            break;
        case KEY_RIGHT:
            if (cursor_pos.second < MAX_XBOUNDARY) {
                if (is_border(cursor_pos.first, cursor_pos.second + 1)) {
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
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::refresh () {
    ::refresh();
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
uint16_t Sudoku::getch () {
    return ::wgetch(stdscr);
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::clear () {
    ::clear();
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
bool Sudoku::is_border (const uint8_t YCOORD, const uint8_t XCOORD) {
    chtype ch = mvinch(YCOORD, XCOORD);
    return ((ch == '|') | (ch == '-'));
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
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
 * Name: 
 * Purpose: 
 * Parameters:
 */
bool Sudoku::do_nothing () {
    //NOTE: Get the 8 cells around the current cursor position
    array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();

    for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
        if ((mvinch(border[i].first, border[i].second) & A_COLOR) == COLOR_PAIR(GIVEN)) return true;
    }
    reset_cursor();
    
    return (inch() & A_COLOR) == COLOR_PAIR(GIVEN);
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::clear_surrounding_cells () {
    //NOTE: Get the 8 cells around the current cursor position
    array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
    for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
        mvprintw(border[i].first, border[i].second, " ");
        //TODO: Make these const later
        uint8_t y = display_matrix_offset[border[i]].first,
                x = display_matrix_offset[border[i]].second;
        display_matrix[y][x] = ' ';
    }
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::place_value (const uint16_t VALUE) {
    /*
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
        //TODO: Make these const later
        uint8_t y = display_matrix_offset[cursor_pos].first,
                x = display_matrix_offset[cursor_pos].second;
                
        reset_cursor();
        chtype ch = inch();
        if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN) or (ch & A_COLOR) == COLOR_PAIR(GUESS)) {
            //TODO: Make these const later?
            uint8_t index = _rev_map_[display_matrix_offset[cursor_pos]],
                    row_number = mat.map_row(index),
                    column_number = mat.map_column(index);
                    
            Row &row = mat.get_row(row_number);
            Column &column = mat.get_column(column_number);
            Box &box = mat.get_box(mat.map_box(row_number, column_number));
            
            if (VALUE == KEY_DC or VALUE == KEY_BACKSPACE) {
                if ((ch & A_COLOR) == COLOR_PAIR(GUESS)) {
                    attron(COLOR_PAIR(UNKNOWN));
                    mvprintw(cursor_pos.first, cursor_pos.second, "?");
                    attroff(COLOR_PAIR(UNKNOWN));
                    
                    row.set_value(mat.get_row_index(index), '?');
                    column.set_value(mat.get_column_index(index), '?');
                    box.set_value(mat.get_box_index(index), '?');
                    display_matrix[y][x] = '?';
                }
                //else if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN)) {}    //Do nothing
            }
            else {
                clear_surrounding_cells();
                attron(COLOR_PAIR(GUESS));
                mvprintw(cursor_pos.first, cursor_pos.second, "%c", VALUE);
                attroff(COLOR_PAIR(GUESS));
                
                //TODO: Maybe put the next 3 lines in Grid.cpp
                row.set_value(mat.get_row_index(index), VALUE);
                column.set_value(mat.get_column_index(index), VALUE);
                box.set_value(mat.get_box_index(index), VALUE);
                display_matrix[y][x] = VALUE;
                
                #if DEBUG
                    ::mvprintw(25, 40 + 20, "index: %d", index);
                    ::mvprintw(26, 40 + 20, "row #: %d", row_number);
                    ::mvprintw(27, 40 + 20, "col #: %d", column_number);
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
                    mat.mvprintw(10, 40 + 20 * i, i & column, i & box);
                    refresh();
                }

                refresh();
            #endif
        }
        else {
            if (VALUE == KEY_DC or VALUE == KEY_BACKSPACE) {
                ::printw(" ");
                display_matrix[y][x] = ' ';
            }
            else {
                //TODO: Now that alternating colors seems to work, update the rest of the code appropriately to account for there being two candidate colors.
                array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
                uint8_t color_pair;
                for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
                    chtype ch = mvinch(border[i].first, border[i].second);
                    if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN) or
                        (ch & A_COLOR) == COLOR_PAIR(GUESS)) {
                        color_pair = (_rev_map_[display_matrix_offset[border[i]]] % 2) ? CANDIDATES_B : CANDIDATES_Y;
                    }
                }
                reset_cursor();
                
                attron(COLOR_PAIR(color_pair));
                attron(A_BOLD);
                ::printw("%c", VALUE);
                attroff(A_BOLD);
                attroff(COLOR_PAIR(color_pair));
                
                display_matrix[y][x] = VALUE;
            }
        }
        refresh();
    }
    reset_cursor(); //NOTE: Have cursor maintain position after printing
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::reset_cursor () {
    ::move(cursor_pos.first, cursor_pos.second);
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
bool Sudoku::evaluate () {
    return mat.evaluate();
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
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
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::save_game () {
    const uint8_t DISPLAY_LINE = ORIGIN.first + DISPLAY_MATRIX_ROWS + 4;
    
    ::move(DISPLAY_LINE, 1);
    clrtoeol();
    ::printw("Enter save file name: ");
    
    uint8_t* display_matrix[DISPLAY_MATRIX_COLUMNS];
    for (uint8_t i = 0; i < DISPLAY_MATRIX_COLUMNS; i++) {
        display_matrix[i] = this->display_matrix[i];
    }
    
    const string NAME = InGameMenu::save_game(display_matrix);
    ::move(DISPLAY_LINE, 1);
    clrtoeol();
    mvprintw(DISPLAY_LINE, ORIGIN.second, "%s saved!", NAME.c_str());
}

/* NOTE:
 * Name: 
 * Purpose: 
 * Parameters:
 */
void Sudoku::start_game (const bool USE_IN_GAME_MENU, const SavedPuzzle* SAVED_PUZZLE) {
    printw(SAVED_PUZZLE);
    InGameMenu* in_game_menu;
    if (not USE_IN_GAME_MENU) {
        in_game_menu = nullptr;
        attron(COLOR_PAIR(MENU_SELECTION));
        ::mvprintw(getmaxy(stdscr) - 1, 0, "s -> save game");
        attroff(COLOR_PAIR(MENU_SELECTION));
    }
    else {
        in_game_menu = new InGameMenu(display_matrix);
        attron(COLOR_PAIR(MENU_SELECTION));
        ::mvprintw(getmaxy(stdscr) - 1, 0, "m -> in-game menu");
        attroff(COLOR_PAIR(MENU_SELECTION));
    }
    ::move(ORIGIN.first, ORIGIN.second);    //starting position of the user
    cursor_pos = make_pair(ORIGIN.first, ORIGIN.second);
    refresh();

    bool quit_game = false;
    do {
        uint16_t input = getch();
        if (tolower(input) == 'q') {    //NOTE: This check has to be here first for this
            quit_game = true;           //      to work as expected. Not sure why.
        }
        else if (tolower(input) == 'm' and USE_IN_GAME_MENU) {
            attron(COLOR_PAIR(MENU_SELECTION));
            mvprintw(getmaxy(stdscr) - 1, 0, "m -> return to game");
            attroff(COLOR_PAIR(MENU_SELECTION));
            clrtoeol();
            
            in_game_menu->menu();
            
            attron(COLOR_PAIR(MENU_SELECTION));
            mvprintw(getmaxy(stdscr) - 1, 0, "m -> in-game menu");
            attroff(COLOR_PAIR(MENU_SELECTION));
            clrtoeol();
            
            reset_cursor();
        }
        //TODO: Add option for 's' when no in-game menu
        else if (tolower(input) == 's' and not USE_IN_GAME_MENU) {
            save_game();
            reset_cursor();
        }
        else if (input >= KEY_DOWN and input <= KEY_RIGHT)  move(input);
        else if (input >= ONE and input <= NINE)            place_value(input);
        else if (input == KEY_DC or input == KEY_BACKSPACE) place_value(input);
        else if (input == KEY_ENTER) {
            if (evaluate()) {
                //TODO: Delete the save file if resuming a game
                string msg = "You win!";
                ::mvprintw(ORIGIN.first + DISPLAY_MATRIX_ROWS + 4, 14, "%s", msg.c_str());
                clrtoeol();
                refresh();
                increment_completed_games();
                quit_game = true;
                this_thread::sleep_for(chrono::seconds(2));
                
                if (SAVED_PUZZLE) filesystem::remove(SAVED_PUZZLE->filename);
            }
            else {
                string msg = "Puzzle incomplete!";  //TODO: Remove this result display after a delay
                ::mvprintw(ORIGIN.first + DISPLAY_MATRIX_ROWS + 4, 14, "%s", msg.c_str());
                refresh();
                reset_cursor();
            }
        }
    } while (!quit_game);
    
    if (USE_IN_GAME_MENU) delete in_game_menu;
}
