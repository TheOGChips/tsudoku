#include "sudoku.hpp"
#include <iostream> //std::cout, std::endl

using namespace std;

const bool DEBUG = true;

Matrix_9x9 sudoku_init()
{
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    map<uint8_t, cell> sudoku_map = create_map();

    if (!DEBUG) {
        cout << "Printing mapping..." << endl;
        for (uint8_t i = 0; i < sudoku_map.size(); i++) {
            cout << "m[" << i+0 << "]: (" << sudoku_map[i].first+0 << ", " << sudoku_map[i].second+0 << ")" << endl;
        }
    }

    Matrix_9x9 mat;
    if (DEBUG) {
        cout << endl
             << "Printing test puzzle..." << endl;
        cout << "SUBMATRIX" << endl;
        mat.print(true, false);
        cout << endl
             << "COLUMNS" << endl;
        mat.print(false, true);
        cout << endl
             << "ROWS" << endl;
        mat.print(false, false);
    }

    /*if (!DEBUG) {
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
