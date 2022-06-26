#include "sudoku.hpp"
#include <iostream>

using namespace std;

Matrix_9x9 sudoku_init()
{
    //TODO: Randomly create a sudoku puzzle
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    //TODO: Randomly fill in sudoku puzzle
        //TODO: Need to be able to print out sudoku puzzle (debug version can be 9x9, at least for now)
        //SOLN: Add overloaded ostream operator as friend function to Matrix_9x9 (and others as necessary) 
    map<uint8_t, cell> sudoku_map = create_map();

    cout << "Printing mapping..." << endl;
    for (uint8_t i = 0; i < sudoku_map.size(); i++) {
        cout << "m[" << i+0 << "]: (" << sudoku_map[i].first+0 << ", " << sudoku_map[i].second+0 << ")" << endl;
    }

    Matrix_9x9 mat;
    cout << endl
         << "Printing empty puzzle..." << endl
         << mat << endl;

    return mat;
}

map<uint8_t, cell> create_map()
{
    map<uint8_t, cell> m;
    //uint8_t count = 0;

    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            m[i*9+j] = pair<uint8_t, uint8_t>(i, j);
            //count++;
        }
    }

    return m;
}
