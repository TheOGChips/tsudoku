#include "Matrix_3x3.hpp"
#include <ncurses.h>

using namespace std;

Matrix_3x3::Matrix_3x3 ()
{
    _map_ = this->create_map(); //to not confuse with similar function in sudoku.hpp
    /*for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            mat[i][j] = '?';
            //mat[i][j] = i * 3 + j + 49;
        }
    }*/

    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, '?');
    }
}

Matrix_3x3::Matrix_3x3 (uint8_t arr[9])
{
    _map_ = this->create_map(); //to not confuse with similar function in sudoku.hpp

    for (uint8_t i = 0; i < 9; i++) {
        set_value(i, arr[i]);
    }
}

map<uint8_t, cell> Matrix_3x3::create_map()
{
    map<uint8_t, cell> m;

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 3; j++) {
            m[i * 3 + j] = cell(i, j);
        }
    }

    return m;
}

uint8_t Matrix_3x3::at (uint8_t index)
{
    return mat[_map_[index].first][_map_[index].second];    //m is an std::pair object
}

uint8_t Matrix_3x3::operator [] (uint8_t index)
{
    return at(index);
}

void Matrix_3x3::print_map()
{
    //cout << m.size() << endl;
    printw("m.size(): %lu", _map_.size());
    for (uint8_t i = 0; i < _map_.size(); i++) {
        //cout << "m[" << i+0 << "]: (" << m[i].first+0 << ", " << m[i].second+0 << ")" << endl;
        printw("m[%u]: (%u, %u)\t", i, _map_[i].first, _map_[i].second);
    }
    printw("\n");
    refresh();
    getch();
    clear();
}

bool Matrix_3x3::value_exists (const uint8_t VALUE)
{
    //value += 48;
    const uint8_t CONVERTED = (VALUE == '?') ? VALUE : VALUE + 48;
    for (uint8_t i = 0; i < 9; i++) {
        if (at(i) == CONVERTED) {
            return true;
        }
    }
    return false;
}

void Matrix_3x3::set_value (uint8_t index, uint8_t value)
{
    mat[_map_[index].first][_map_[index].second] = value;
}

bool Matrix_3x3::evaluate() {
    if (value_exists('?')) return false;
    for (uint8_t i = 1; i <= 9; i++) {
        if (not value_exists(i)) return false;
    }
    return true;
}
