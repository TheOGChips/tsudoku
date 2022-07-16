#ifndef MATRIX_9X9_HPP
#define MATRIX_9X9_HPP

#include "values.hpp"
#include "Matrix_3x3.hpp"
#include "Row.hpp"
#include "Column.hpp"
#include <random>   //mt19937(), uniform_int_distribution<>(),
#include "colors.hpp"

using namespace std;

class Matrix_9x9 {
    private:
        Matrix_3x3 matrices[9];
        Row rows[9];
        Column cols[9];
        bool positions[81];
        mt19937 position_generator,
                generator;
        uniform_int_distribution<uint8_t> position_dist,
                                          dist;

        void init_positions();
        void set_starting_positions (uint8_t);
        uint8_t next_position();
        uint8_t map_row (const uint8_t);
        uint8_t map_column (const uint8_t);
        uint8_t map_submatrix (const uint8_t, const uint8_t);
        uint8_t next_value();
        void get_indeces (const uint8_t, uint8_t&, uint8_t&, uint8_t&);

    public:
        Matrix_9x9();
        ~Matrix_9x9();
        Matrix_3x3& get_submatrix (uint8_t);
        Row& get_row (uint8_t);
        Column& get_column (uint8_t);
        //void print(const bool, const bool);
        void printw(const bool, const bool);    //the one to call while using ncurses
        //friend ostream& operator << (ostream&, Matrix_9x9);
};

#endif
