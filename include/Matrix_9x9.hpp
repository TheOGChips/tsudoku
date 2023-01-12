#ifndef MATRIX_9X9_HPP
#define MATRIX_9X9_HPP

//#include "values.hpp"
#include "Matrix_3x3.hpp"
#include "Row.hpp"
#include "Column.hpp"
#include <random>   //mt19937(), uniform_int_distribution<>(),
#include <array>

using namespace std;

class Matrix_9x9 {
    private:
        Matrix_3x3 matrices[9];
        Row rows[9];
        Column cols[9];
        bool known_positions[81];
        //mt19937 //position_generator,
                //generator;
        //uniform_int_distribution<uint8_t> //position_dist,
                                          //dist;
        map<uint8_t, cell> _map_;   //maps 81 positions to positions on 9x9 matrix

        void init_positions();
        array<uint8_t, 81> generate_solved_puzzle (time_t);
        void set_starting_positions (uint8_t);
        //uint8_t next_position();
        //uint8_t next_value();
        map<uint8_t, cell> create_map();
        bool solve(uint8_t, uint8_t, Row[9], Column[9], Matrix_3x3[9]);

    public:
        Matrix_9x9();
        ~Matrix_9x9() {}
        uint8_t map_row (const uint8_t);
        uint8_t map_column (const uint8_t);
        uint8_t map_submatrix (const uint8_t, const uint8_t);
        Matrix_3x3& get_submatrix (uint8_t);
        Row& get_row (uint8_t);
        Column& get_column (uint8_t);
        uint8_t get_row_index (const uint8_t);
        uint8_t get_column_index (const uint8_t);
        uint8_t get_submatrix_index (const uint8_t);
        //void print(const bool, const bool);
        void mvprintw (const uint8_t, const uint8_t, const bool, const bool);    //the one to call while using ncurses
        void printw (const bool, const bool);
        const cell get_map (uint8_t);
        uint8_t get_map_size() const;
        uint8_t at (uint8_t);
        uint8_t operator [] (uint8_t);
        bool is_known (uint8_t);
        bool evaluate();    //TODO: Add function for evaluating puzzle after the user hits ENTER
};

#endif
