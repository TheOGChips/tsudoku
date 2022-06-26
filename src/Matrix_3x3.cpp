#include "Matrix_3x3.hpp"
#include <iostream>

Matrix_3x3::Matrix_3x3 ()
{
    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            //mat[i][j] = '-';
            mat[i][j] = i * 3 + j + 49;
        }
    }

    m = this->create_map(); //to not confuse with similar function in sudoku.hpp
}

map<uint8_t, cell> Matrix_3x3::create_map()
{
    map<uint8_t, cell> m;
    //uint8_t count = 0;

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            m[i*3+j] = cell(i, j);
        }
    }

    return m;
}

const uint8_t Matrix_3x3::operator [] (uint8_t index)
{
    return mat[m[index].first][m[index].second];
}

void Matrix_3x3::print_map()
{
    cout << m.size() << endl;
    for (uint8_t i = 0; i < m.size(); i++) {
        cout << "m[" << i+0 << "]: (" << m[i].first+0 << ", " << m[i].second+0 << ")" << endl;
    }
}
