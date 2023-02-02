#include <ncurses.h>
#include "sudoku.hpp"
#include "colors.hpp"
#include <cctype>
#include <thread>
#include <chrono>
#include <fstream>
#include "InGameMenu.hpp"

using namespace std;

const bool DEBUG = false;

//Grid sudoku_init()
Sudoku::Sudoku (bool is_initscr)
{
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    create_map();

    //Start ncurses
    
    if (not is_initscr) initscr();
    //establish color support and color pairs
    set_color_pairs();
    cbreak();   //TODO: Will need to account for signal handling
    noecho();
    keypad(stdscr, true);

    if (DEBUG) {
        //cout <<  << endl;
        ::printw("Printing mapping...\n");
        for (uint8_t i = 0; i < mat.get_map_size(); i++) {
            //cout << "m[" << i+0 << "]: (" << get_map(i).first+0 << ", " << get_map(i).second+0 << ")" << endl;
            ::printw("m[%u]: (%u, %u)", i, mat.get_map(i).first, mat.get_map(i).second);
            (i+1) % 9 ? ::printw("\t") : ::printw("\n");
        }
        ::printw("\n\n");
        for (uint8_t i = 0; i < _map_.size(); i++) {
            //cout << "m[" << i+0 << "]: (" << get_map(i).first+0 << ", " << get_map(i).second+0 << ")" << endl;
            ::printw("m[%u]: (%u, %u)", i, _map_[i].first, _map_[i].second);
            (i+1) % 9 ? ::printw("\t") : ::printw("\n");
        }
        ::printw("\n\n");
        for (uint8_t i = 0; i < _rev_map_.size(); i++) {
            ::printw("rm[(%u, %u)]:\t%u", _map_[i].first, _map_[i].second, _rev_map_[_map_[i]]);
            (i+1) % 9 ? ::printw("\t") : ::printw("\n");
        }
        //NOTE: The mapping appears to be correct according to the printout, so why is adding new values not working as expected?
        refresh();  //TODO: Consider putting these three functions into one if used like this more often
        getch();
        clear();
    }

    if (DEBUG) {
        enum print_by {row, column, submatrix};
        for (uint8_t i = row; i <= submatrix; i++) {
            ::printw("Printing by ");
            if (i == submatrix) {
                ::printw("submatrix...\n");
            }
            else if (i == column) {
                ::printw("column...\n");
            }
            else {
                ::printw("row...\n");
            }
            mat.printw(i & column, i & submatrix);
            refresh();  //flush output to screen
            getch();    //wait for user input
            clear();    //clear the screen
        }
    }

    /*if (DEBUG) {
        cout << "Printing out random numbers..." << endl;
        for (uint8_t i = 0; i < 10; i++) {
            cout << mat.next()+0 << endl;
        }
    }*/

    init_display_matrix();
}

Sudoku::~Sudoku()
{
    echo();
    nocbreak();
    endwin();   //terminate ncurses session
}

//map<uint8_t, cell> Sudoku::create_map()
void Sudoku::create_map()
{
    //map<uint8_t, cell> m;
    uint8_t row = 1,
            column = 1;

    for (uint8_t i = 0; i < mat.get_map_size(); i++) {
        //m[i] = cell(row, column);
        _map_[i] = cell(row, column);
        _rev_map_[cell(row, column)] = i;
        column += 3;
        if (column / 27) {
            column %= 27;
            row += 3;
        }
    }

    //return m;
}

void Sudoku::map_display_matrix_offset (const uint8_t YINDEX, const uint8_t XINDEX)
{
    uint8_t y,
            x;
    getyx(stdscr, y, x);
    cell display_indeces = {YINDEX, XINDEX},
         coords = {y, x};
    display_matrix_offset[coords] = display_indeces;
}

void Sudoku::set_color_pairs()
{ 
    start_color();  //NOTE: I'm guessing this should work like this, but I don't have a non-color-supported
                    //      terminal to test this out on, and this is the simplest thing to do without adding
                    //      checks everywhere. If someone else knows or finds that this function doesn't work
                    //      as intended, feel free to correct it.
    if (has_colors()) { //color mode
        //init_pair(UNKNOWN, COLOR_BLACK, COLOR_WHITE);
        init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
        init_pair(GIVEN, COLOR_RED, COLOR_BLACK);
        init_pair(CANDIDATES, COLOR_YELLOW, COLOR_BLACK);
        init_pair(GUESS, COLOR_GREEN, COLOR_BLACK);
        init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
    }
    else {  //monochrome mode
        init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
        init_pair(GIVEN, COLOR_WHITE, COLOR_BLACK);
        init_pair(CANDIDATES, COLOR_WHITE, COLOR_BLACK);
        init_pair(GUESS, COLOR_WHITE, COLOR_BLACK);
        init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
    }
}

void Sudoku::init_display_matrix()
{
    /*
         0,0  0,1  0,2  0,3  0,4  0,5  0,6  0,7  0,8 |  0,9  0,10  0,11  0,12  0,13  0,14  0,15  0,16  0,17 |  0,18  0,19  0,20  0,21  0,22  0,23  0,24  0,25  0,26
         1,0  1,1            1,4            1,7      |       1,10              1,13              1,16       |        1,19              1,22              1,25
         2,0                                         |                                                      |
         3,0                                         |                                                      |
         4,0  4,1            4,4            4,7      |       4,10              4,13              4,16       |        4,19              4,22              4,25
         5,0                                         |                                                      |
         6,0                                         |                                                      |
         7,0  7,1            7,4            7,7      |       7,10              7,13              7,16       |        7,19              7,22              7,25
         8,0                                         |                                                      |
         --------------------------------------------|------------------------------------------------------|------------------------------------------------------
         9,0                                         |                                                      |
        10,0 10,1           10,4           10,7      |      10,10             10,13             10,16       |       10,19             10,22             10,25
        11,0                                         |                                                      |
        12,0                                         |                                                      |
        13,0 13,1           13,4           13,7      |      13,10             13,13             13,16       |       13,19             13,22             13,25
        14,0                                         |                                                      |
        15,0                                         |                                                      |
        16,0 16,1           16,4           16,7      |      16,10             16,13             16,16       |       16,19             16,22             16,25
        17,0                                         |                                                      |
        ---------------------------------------------|------------------------------------------------------|------------------------------------------------------
        18,0                                         |                                                      |
        19,0 19,1           19,4           19,7      |      19,10             19,13             19,16       |       19,19             19,22             19,25
        20,0                                         |                                                      |
        21,0                                         |                                                      |
        22,0 22,1           22,4           22,7      |      22,10             22,13             22,16       |       22,19             22,22             22,25
        23,0                                         |                                                      |
        24,0                                         |                                                      |
        25,0 25,1           25,4           25,7      |      25,10             25,13             25,16       |       25,19             25,22             25,25
        26,0                                         |                                                      |
    */

    //initialize display matrix with blank spaces
    for (uint8_t i = 0; i < 27; i++) {
        for (uint8_t j = 0; j < 27; j++) {
            display_matrix[i][j] = ' ';
        }
    }

    for (uint8_t i = 0; i < _map_.size(); i++) {
        cell coords = _map_[i];
        display_matrix[coords.first][coords.second] = mat[i];
    }
}

void Sudoku::printw (/*const bool COLUMN_PRINTING, const bool SUBMATRIX_PRINTING*/)
{
    if (DEBUG) {
        ::printw("Printing display matrix...\n");
    }

    //::move(INIT_OFFSETY, INIT_OFFSETX);
    for (uint8_t i = 0; i < 27; i++) {
        move(i, 0);
        for (uint8_t j = 0; j < 27; j++) {
            map_display_matrix_offset(i, j);
            ::printw("%c", display_matrix[i][j]);
            if (j == 8 or j == 17) {
                ::printw("|");
            }
        }
        //::printw("\n");
        if (i == 8 or i == 17) {
            //::move(i + INIT_OFFSETY + (i / 9) + 1, INIT_OFFSETX);
            ::move(i + ORIGIN.first + (i / 9) + 1, ORIGIN.second);
            //move(i, 0);
            //::printw("---------|---------|---------\n");
            ::printw("---------|---------|---------");
        }
    }

    //NOTE: Not needed currently, but probably will be later
    /*uint16_t posx,
             posy;
    getyx(stdscr, posy, posx);*/
    for (uint8_t i = 0; i < _map_.size(); i++) {
        cell coords = _map_[i];
        move(coords.first, coords.second);  //Move cursor to position
        if (mat.is_known(i)) {
            attron(COLOR_PAIR(GIVEN));  //Turn color scheme on
            ::printw("%c", display_matrix[coords.first][coords.second]);    //Print value
            attroff(COLOR_PAIR(GIVEN));//Turn color scheme off
        }
        else {
            attron(COLOR_PAIR(UNKNOWN));
            ::printw("%c", display_matrix[coords.first][coords.second]);    //Print value
            attroff(COLOR_PAIR(UNKNOWN));
        }
    }
}

void Sudoku::move (const uint8_t YCOORD, const uint8_t XCOORD)
{
    uint8_t total_offsety = YCOORD + ORIGIN.first + (YCOORD / 9),
            total_offsetx = XCOORD + ORIGIN.second + (XCOORD / 9);

    ::move(total_offsety, total_offsetx);
    getyx(stdscr, cursor_pos.first, cursor_pos.second); //update cursor_pos after moving
}

void Sudoku::move (const uint16_t KEY)
{
    static const uint8_t MAX_YBOUNDARY = ORIGIN.first + 28,
                         MAX_XBOUNDARY = ORIGIN.second + 28;

    switch (KEY) {
        case KEY_DOWN:  if (cursor_pos.first < MAX_YBOUNDARY) {
                            if (is_border(cursor_pos.first + 1, cursor_pos.second)) {
                                ::move(cursor_pos.first + 2, cursor_pos.second);
                            }
                            else {
                                ::move(cursor_pos.first + 1, cursor_pos.second);
                            }
                            getyx(stdscr, cursor_pos.first, cursor_pos.second);
                        }
                        break;
        case KEY_UP:    if (cursor_pos.first > ORIGIN.first) {
                            if (is_border(cursor_pos.first - 1, cursor_pos.second)) {
                                ::move(cursor_pos.first - 2, cursor_pos.second);
                            }
                            else {
                                ::move(cursor_pos.first - 1, cursor_pos.second);
                            }
                            getyx(stdscr, cursor_pos.first, cursor_pos.second);
                        }
                        break;
        case KEY_LEFT:  if (cursor_pos.second > ORIGIN.second) {
                            if (is_border(cursor_pos.first, cursor_pos.second - 1)) {
                                ::move(cursor_pos.first, cursor_pos.second - 2);
                            }
                            else {
                                ::move(cursor_pos.first, cursor_pos.second - 1);
                            }
                            getyx(stdscr, cursor_pos.first, cursor_pos.second);
                        }
                        break;
        case KEY_RIGHT: if (cursor_pos.second < MAX_XBOUNDARY) {
                            if (is_border(cursor_pos.first, cursor_pos.second + 1)) {
                                ::move(cursor_pos.first, cursor_pos.second + 2);
                            }
                            else {
                                ::move(cursor_pos.first, cursor_pos.second + 1);
                            }
                            getyx(stdscr, cursor_pos.first, cursor_pos.second);
                        }
                        break;
        default:;   //This shouldn't ever be run because the of the switch statement in start_game
    }
}

void Sudoku::refresh ()
{
    ::refresh();
}

uint16_t Sudoku::getch()
{
    return ::wgetch(stdscr);
}

void Sudoku::clear()
{
    ::clear();
}

bool Sudoku::is_border (const uint8_t YCOORD, const uint8_t XCOORD)
{
    chtype ch = mvinch(YCOORD, XCOORD);
    return ((ch == '|') | (ch == '-'));
}

array<cell, Sudoku::NUM_BORDER_POSITIONS> Sudoku::get_surrounding_cells()
{
    //reset_cursor();
    //NOTE: This has to be done this way, or else the array initialization doesn't work for some weird
    //      reason.
    cell tl = {cursor_pos.first - 1, cursor_pos.second - 1};
    cell t  = {cursor_pos.first - 1, cursor_pos.second};
    cell tr = {cursor_pos.first - 1, cursor_pos.second + 1};
    cell l  = {cursor_pos.first,     cursor_pos.second - 1};
    cell r  = {cursor_pos.first,     cursor_pos.second + 1};
    cell bl = {cursor_pos.first + 1, cursor_pos.second - 1};
    cell b  = {cursor_pos.first + 1, cursor_pos.second};
    cell br = {cursor_pos.first + 1, cursor_pos.second + 1};
    array<cell, NUM_BORDER_POSITIONS> border = {tl, t, tr, l, r, bl, b, br};

    /*::mvprintw(4, 40, "%d,%d", cursor_pos.first - 1, cursor_pos.second - 1);
    ::mvprintw(5, 40, "%d,%d", border[TL].first, border[TL].second);
    ::mvprintw(5, 46, "%d,%d", border[T].first, border[T].second);
    ::mvprintw(5, 52, "%d,%d", border[TR].first, border[TR].second);
    ::mvprintw(6, 40, "%d,%d", border[L].first, border[L].second);
    ::mvprintw(6, 46, "%d,%d", cursor_pos.first, cursor_pos.second);
    ::mvprintw(6, 52, "%d,%d", border[R].first, border[R].second);
    ::mvprintw(7, 40, "%d,%d", border[BL].first, border[BL].second);
    ::mvprintw(7, 46, "%d,%d", border[B].first, border[B].second);
    ::mvprintw(7, 52, "%d,%d", border[BR].first, border[BR].second);
    refresh();
    getch();*/

    return border;
}

bool Sudoku::do_nothing()
{
    // Get the 8 cells around the current cursor position
    array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
    //chtype ch = inch();

    /*::mvprintw(5, 40, "%d,%d", border[TL].first, border[TL].second);
    ::mvprintw(5, 46, "%d,%d", border[T].first, border[T].second);
    ::mvprintw(5, 52, "%d,%d", border[TR].first, border[TR].second);
    ::mvprintw(6, 40, "%d,%d", border[L].first, border[L].second);
    ::mvprintw(6, 46, "%d,%d", cursor_pos.first, cursor_pos.second);
    ::mvprintw(6, 52, "%d,%d", border[R].first, border[R].second);
    ::mvprintw(7, 40, "%d,%d", border[BL].first, border[BL].second);
    ::mvprintw(7, 46, "%d,%d", border[B].first, border[B].second);
    ::mvprintw(7, 52, "%d,%d", border[BR].first, border[BR].second);
    refresh();
    getch();*/

    //if ((inch() & A_COLOR) == COLOR_PAIR(GIVEN)) return true;
    for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
        if ((mvinch(border[i].first, border[i].second) & A_COLOR) == COLOR_PAIR(GIVEN)) return true;
    }
    reset_cursor();
    //return ((inch() & A_COLOR) == COLOR_PAIR(GIVEN)) ? true : false;
    return (inch() & A_COLOR) == COLOR_PAIR(GIVEN);
    /*return ((mvinch(TL.first, TL.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch( T.first,  T.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch(TR.first, TR.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch( L.first,  L.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((ch & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch( R.first,  R.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch(BL.first, BL.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch( B.first,  B.second) & A_COLOR) == COLOR_PAIR(GIVEN)) |
           ((mvinch(BR.first, BR.second) & A_COLOR) == COLOR_PAIR(GIVEN));*/
}

void Sudoku::clear_surrounding_cells()
{
    //reset_cursor();
    // Get the 8 cells around the current cursor position
    array<cell, NUM_BORDER_POSITIONS> border = get_surrounding_cells();
    /*::mvprintw(5, 40, "%d,%d", border[TL].first, border[TL].second);
    ::mvprintw(5, 46, "%d,%d", border[T].first, border[T].second);
    ::mvprintw(5, 52, "%d,%d", border[TR].first, border[TR].second);
    ::mvprintw(6, 40, "%d,%d", border[L].first, border[L].second);
    ::mvprintw(6, 46, "%d,%d", cursor_pos.first, cursor_pos.second);
    ::mvprintw(6, 52, "%d,%d", border[R].first, border[R].second);
    ::mvprintw(7, 40, "%d,%d", border[BL].first, border[BL].second);
    ::mvprintw(7, 46, "%d,%d", border[B].first, border[B].second);
    ::mvprintw(7, 52, "%d,%d", border[BR].first, border[BR].second);
    refresh();*/
    for (uint8_t i = TL; i < NUM_BORDER_POSITIONS; i++) {
        mvprintw(border[i].first, border[i].second, " ");
    }
    //reset_cursor();
}

void Sudoku::place_value (const uint16_t VALUE)
{
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
     *      place into appropriate spot in appropriate row, column, and 3x3 submatrix
     */
    //clear();
    // Get the 8 cells around the current cursor position
    /*cell TL = {cursor_pos.first - 1, cursor_pos.second - 1},
         T  = {cursor_pos.first - 1, cursor_pos.second},
         TR = {cursor_pos.first - 1, cursor_pos.second + 1},
         L  = {cursor_pos.first,     cursor_pos.second - 1},
         R  = {cursor_pos.first,     cursor_pos.second + 1},
         BL = {cursor_pos.first + 1, cursor_pos.second - 1},
         B  = {cursor_pos.first + 1, cursor_pos.second},
         BR = {cursor_pos.first + 1, cursor_pos.second + 1};*/

    //::mvprintw(20, 100, "ch: %u\n", ch);
    //::mvprintw(21, 100, "COLOR_RED: %d\n", COLOR_RED);
    //::mvprintw(22, 100, "A_COLOR: %d\n", A_COLOR);
    //::mvprintw(23, 100, "COLOR_PAIR(GIVEN): %d\n", COLOR_PAIR(GIVEN));
    //::mvprintw(24, 100, "ch & A_COLOR: %d\n", ch & A_COLOR);
    //::refresh();
    if (do_nothing()) reset_cursor();
    //else if ((ch & A_CHARTEXT) == '?') {}
    else {
        reset_cursor();
        chtype ch = inch();
        if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN) or (ch & A_COLOR) == COLOR_PAIR(GUESS)) {
            if (VALUE == KEY_DC or VALUE == KEY_BACKSPACE) {
                if ((ch & A_COLOR) == COLOR_PAIR(GUESS)) {
                    attron(COLOR_PAIR(UNKNOWN));
                    mvprintw(cursor_pos.first, cursor_pos.second, "?");
                    attroff(COLOR_PAIR(UNKNOWN));
                }
                //else if ((ch & A_COLOR) == COLOR_PAIR(UNKNOWN)) {}    //Do nothing
            }
            else {
                clear_surrounding_cells();
                attron(COLOR_PAIR(GUESS));
                mvprintw(cursor_pos.first, cursor_pos.second, "%c", VALUE);
                attroff(COLOR_PAIR(GUESS));

                uint8_t index = _rev_map_[display_matrix_offset[cursor_pos]],
                        row_number = mat.map_row(index),
                        column_number = mat.map_column(index);

                //TODO: Maybe put the next 7 lines in Grid.cpp
                Row &row = mat.get_row(row_number);
                Column &column = mat.get_column(column_number);
                Box &submatrix = mat.get_submatrix(mat.map_submatrix(row_number, column_number));

                row.set_value(mat.get_row_index(index), VALUE);
                column.set_value(mat.get_column_index(index), VALUE);
                submatrix.set_value(mat.get_submatrix_index(index), VALUE);

                if (DEBUG) {
                    ::mvprintw(25, 40 + 20, "index: %d", index);
                    ::mvprintw(26, 40 + 20, "row #: %d", row_number);
                    ::mvprintw(27, 40 + 20, "col #: %d", column_number);
                }
            }

            if (DEBUG) {
                enum print_by {row, column, submatrix};
                for (uint8_t i = row; i <= submatrix; i++) {
                    //::mvprintw(9, 40 + 20 * i, "Printing by ");
                    ::move(9, 40 + 20 * i);
                    if (i == submatrix) {
                        ::printw("submatrix");
                    }
                    else if (i == column) {
                        ::printw("column");
                    }
                    else {
                        ::printw("row");
                    }
                    mat.mvprintw(10, 40 + 20 * i, i & column, i & submatrix);
                    refresh();  //flush output to screen
                    //getch();    //wait for user input
                    //clear();    //clear the screen
                }

                refresh();
            }
        }
        else {
            if (VALUE == KEY_DC or VALUE == KEY_BACKSPACE) {
                ::printw(" ");
            }
            else {
                //TODO: Try to figure out how to alternate colors
                attron(COLOR_PAIR(CANDIDATES));
                attron(A_BOLD);
                ::printw("%c", VALUE);
                attroff(A_BOLD);
                attroff(COLOR_PAIR(CANDIDATES));
            }
        }

        uint8_t y = display_matrix_offset[cursor_pos].first,
                x = display_matrix_offset[cursor_pos].second;
        display_matrix[y][x] = VALUE;
        refresh();
    }
    reset_cursor(); //have cursor maintain position after printing (maybe unnecessary now)
}



void Sudoku::reset_cursor ()
{
    ::move(cursor_pos.first, cursor_pos.second);
}

bool Sudoku::evaluate() {
    return mat.evaluate();
}

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

void Sudoku::save_game () {
    string msg = "Enter save file name: ";
    const uint8_t NAME_SIZE = 16,
                  DISPLAY_LINE = ORIGIN.first + 31;
    char name[NAME_SIZE];
    
    ::move(DISPLAY_LINE, 1);
    clrtoeol();
    ::printw("%s", msg.c_str());
    echo();
    getnstr(name, NAME_SIZE - 1);
    noecho();
    
    const string FILENAME = DIR + "/" + name + ".csv";
    ofstream outfile;
    outfile.open(FILENAME.c_str());
    for (uint8_t i = 0; i < DISPLAY_MATRIX_SIZE; i++) {
        for (uint8_t j = 0; j < DISPLAY_MATRIX_SIZE; j++) {
            outfile << static_cast<uint16_t>(display_matrix[i][j]);
            if (j < DISPLAY_MATRIX_SIZE - 1) outfile << ",";
        }
        outfile << endl;
    }
    outfile.close();
    
    ::move(DISPLAY_LINE, 0);
    clrtoeol();
    mvprintw(DISPLAY_LINE, ORIGIN.second, "%s saved!", name);
}

void Sudoku::start_game (const bool USE_IN_GAME_MENU)
{
    //Load and display the new or saved puzzle
    printw();
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

    //const uint8_t KEY_ENTER = 10;   //NOTE: This is the Enter key on the main keyboard. The original
    bool quit_game = false;         //      KEY_ENTER refers to the one on the number pad, but that 
                                    //      one doesn't seem to work as expected anyway.
    
    do {
        uint16_t input = getch();
        if (tolower(input) == 'q') {    //NOTE: This check has to be here first for this
            quit_game = true;           //      to work. Not sure why.
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
        else if (input >= KEY_DOWN and input <= KEY_RIGHT) {
            move(input);
        }
        else if (input >= ONE and input <= NINE) {
            place_value(input);
        }
        else if (input == KEY_DC or input == KEY_BACKSPACE) {
            place_value(input);
        }
        else if (input == KEY_ENTER) {
            #if false
            uint8_t y = display_matrix_offset[cursor_pos].first,
                    x = display_matrix_offset[cursor_pos].second;
            ::mvprintw(20, 100, "    cursor_pos[%d][%d]: ", cursor_pos.first, cursor_pos.second);
            ::mvprintw(21, 100, "display_matrix[%d][%d]: %d", y, x, display_matrix[y][x]);
            refresh();
            reset_cursor();
            #endif
            if (evaluate()) {
                string msg = "You win!";
                ::mvprintw(ORIGIN.first + 31, 14, "%s", msg.c_str());
                clrtoeol();
                refresh();
                increment_completed_games();
                quit_game = true;
                this_thread::sleep_for(chrono::seconds(2));
            }
            else {
                string msg = "Puzzle incomplete!";  //TODO: Remove this result display after a delay
                ::mvprintw(ORIGIN.first + 31, 14, "%s", msg.c_str());
                refresh();
                reset_cursor();
            }
        }
        
    } while (!quit_game);
    
    if (USE_IN_GAME_MENU) delete in_game_menu;
}
