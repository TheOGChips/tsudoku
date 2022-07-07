#include "sudoku.hpp"
#include <iostream>

using namespace std;

const bool DEBUG = true;

Matrix_9x9 sudoku_init()
{
    //TODO: Randomly create a sudoku puzzle
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    //TODO: Randomly fill in sudoku puzzle
        //TODO: Determine how to randomly fill in sudoku puzzle
        //SOLN: step 1 -> pick position (on 9x9 grid) <- TODO
        //      step 2 -> map position to corresponding 3x3 grid, row, and column <- TODO
        //      step 3 -> pick random character between 1-9 to fill in spot <- TODO
        //      step 4 -> place random character in appropriate spot in corresponding 3x3 grid, row, and column <- TODO
    map<uint8_t, cell> sudoku_map = create_map();

    if (!DEBUG) {
        cout << "Printing mapping..." << endl;
        for (uint8_t i = 0; i < sudoku_map.size(); i++) {
            cout << "m[" << i+0 << "]: (" << sudoku_map[i].first+0 << ", " << sudoku_map[i].second+0 << ")" << endl;
        }
    }

    Matrix_9x9 mat;
    if (!DEBUG) {
        cout << endl
             << "Printing test puzzle..." << endl
             << mat << endl;
    }

    if (!DEBUG) {
        cout << "Printing out random numbers..." << endl;
        for (uint8_t i = 0; i < 10; i++) {
            cout << mat.next()+0 << endl;
        }
    }

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
