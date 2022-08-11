#include "sudoku.hpp"
#include <ncurses.h>
#include "colors.hpp"

using namespace std;

const bool DEBUG = true;

//Matrix_9x9 sudoku_init()
Sudoku::Sudoku ()
{
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    //map<cell, cell> display_map = create_map();

    //Start ncurses
    initscr();
    //establish color support and color pairs
    set_color_pairs();

    if (DEBUG) {
        //cout <<  << endl;
        ::printw("Printing mapping...\n");
        for (uint8_t i = 0; i < mat.get_map_size(); i++) {
            //cout << "m[" << i+0 << "]: (" << get_map(i).first+0 << ", " << get_map(i).second+0 << ")" << endl;
            ::printw("m[%u]: (%u, %u)", i, mat.get_map(i).first, mat.get_map(i).second);
            (i+1) % 9 ? ::printw("\t") : ::printw("\n");
        }
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
}

Sudoku::~Sudoku()
{
    endwin();   //terminate ncurses session
}

/*map<cell, cell> create_map()
{
}*/

void Sudoku::set_color_pairs()
{ 
    start_color();  //NOTE: I'm guessing this should work like this, but I don't have a non-color-supported
                    //      terminal to test this out on, and this is the simplest thing to do without adding
                    //      checks everywhere. If someone else knows or finds that this function doesn't work
                    //      as intended, feel free to correct it.
    if (has_colors()) { //color mode
        init_pair(UNKNOWN, COLOR_BLACK, COLOR_WHITE);
        init_pair(KNOWN, COLOR_RED, COLOR_BLACK);
        init_pair(GUESS, COLOR_GREEN, COLOR_BLACK);
    }
    else {  //monochrome mode
        init_pair(UNKNOWN, COLOR_WHITE, COLOR_BLACK);
        init_pair(KNOWN, COLOR_WHITE, COLOR_BLACK);
        init_pair(GUESS, COLOR_WHITE, COLOR_BLACK);
    }
}

void Sudoku::init_display_matrix()
{
    /*  TODO: Will need a way to map the 9x9 matrix to the special values in the display matrix
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
}

void Sudoku::printw (/*const bool COLUMN_PRINTING, const bool SUBMATRIX_PRINTING*/)
{
}
