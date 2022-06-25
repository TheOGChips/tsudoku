#include "sudoku.hpp"
#include <iostream>

using namespace std;

Matrix_9x9 sudoku_init()
{
    //TODO: Randomly create a sudoku puzzle
    //NOTE: According to https://www.101computing.net/sudoku-generator-algorithm/, the minimum amount of tiles that need to be
    //      filled in in order to create a uniquely-solvable puzzle is 17 (this will later be HARD difficulty if diffuculty
    //      settings are added later)
    //TODO: Need a way to map each of the numbers 0-80 to a cell in the 9x9 sudoku matrix
    //SOLN: Use a map and a pair
    map<uint8_t, cell> sudoku_map = create_map();

    cout << "Printing mapping..." << endl;
    for (uint8_t i = 0; i < sudoku_map.size(); i++) {
        cout << "m[" << i+0 << "]: (" << sudoku_map[i].first+0 << ", " << sudoku_map[i].second+0 << ")" << endl;
    }

    return Matrix_9x9();
}

map<uint8_t, cell> create_map()
{
    map<uint8_t, cell> m;
    uint8_t count = 0;

    for (uint8_t i = 0; i < 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            m[count] = pair<uint8_t, uint8_t>(i, j);
            count++;
        }
    }

    return m;
}
