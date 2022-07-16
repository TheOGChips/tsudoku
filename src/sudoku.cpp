#include "sudoku.hpp"
#include <ncurses.h>

using namespace std;

const bool DEBUG = true;

Matrix_9x9 sudoku_init()
{
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    map<uint8_t, cell> sudoku_map = create_map();

    if (!DEBUG) {
        //cout <<  << endl;
        printw("Printing mapping...\n");
        for (uint8_t i = 0; i < sudoku_map.size(); i++) {
            //cout << "m[" << i+0 << "]: (" << sudoku_map[i].first+0 << ", " << sudoku_map[i].second+0 << ")" << endl;
            printw("m[%u]: (%u, %u)", i, sudoku_map[i].first, sudoku_map[i].second);
            i % 10 ? printw("\t") : printw("\n");
        }
        refresh();  //TODO: Consider putting these three functions into one if used like this more often
        getch();
        clear();
    }

    Matrix_9x9 mat;
    if (DEBUG) {
        enum print_by {row, column, submatrix};
        for (uint8_t i = row; i <= submatrix; i++) {
            printw("Printing by ");
            if (i == submatrix) {
                printw("submatrix...\n");
            }
            else if (i == column) {
                printw("column...\n");
            }
            else {
                printw("row...\n");
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

    return mat;
}

map<uint8_t, cell> create_map()
{
    map<uint8_t, cell> m;

    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            m[i * 9 + j] = pair<uint8_t, uint8_t>(i, j);
        }
    }

    return m;
}
