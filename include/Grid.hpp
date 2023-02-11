#ifndef GRID_HPP
#define GRID_HPP

#include "Container.hpp"    //Box, Column, Row
#include <random>           //mt19937(), uniform_int_distribution<>(),
#include <array>            //std::array
#include <map>              //std::map

using namespace std;

const uint8_t GRID_SIZE = 81;

/* NOTE:
 * Class: Grid
 * Purpose: Represents a 9x9 Sudoku grid.
 * Private variables:
 *      arr -> Internal array housing the container's contents.
 */
class Grid {
    private:
        Box boxes[NUM_CONTAINERS];
        Row rows[NUM_CONTAINERS];
        Column cols[NUM_CONTAINERS];
        bool known_positions[GRID_SIZE];
        map<uint8_t, cell> grid_map;   //NOTE: Maps 81 positions to positions on 9x9 matrix

        void init_positions();
        array<uint8_t, GRID_SIZE> generate_solved_puzzle (time_t);
        void set_starting_positions (uint8_t);
        void set_starting_positions (const uint8_t[NUM_CONTAINERS][NUM_CONTAINERS]);
        map<uint8_t, cell> create_map();
        bool solve(uint8_t, uint8_t, Row[NUM_CONTAINERS], Column[NUM_CONTAINERS], Box[NUM_CONTAINERS]);

    public:
        //TODO: A lot of these can probably be moved to private now
        Grid () {}
        Grid (const uint8_t[NUM_CONTAINERS][NUM_CONTAINERS]);
        Grid (const difficulty_level);
        ~Grid() {}
        uint8_t map_row (const uint8_t);
        uint8_t map_column (const uint8_t);
        uint8_t map_submatrix (const uint8_t, const uint8_t);
        Box& get_submatrix (uint8_t);
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
        bool evaluate();
};

#endif
