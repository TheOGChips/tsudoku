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
 *      boxes -> Array of each Box container in the grid.
 *      rows -> Array of each Row container in the grid.
 *      cols -> Array of each Column container in the grid.
 *      known_positions -> Boolean array of known positions in the grid. Positions are numbered 0-80.
 *                         True values will correspond to cells with given values.
 *      grid_map -> Map of 81 sudoku grid positions (0-80) to (i,j) cell positions. i and j both lie
 *                  in the range [0-8].
 */
class Grid {
    private:
        Box boxes[NUM_CONTAINERS];
        Row rows[NUM_CONTAINERS];
        Column cols[NUM_CONTAINERS];
        bool known_positions[GRID_SIZE];
        map<uint8_t, cell> grid_map;   //NOTE: Maps 81 positions to positions on 9x9 matrix

        void init_known_positions ();
        array<uint8_t, GRID_SIZE> generate_solved_puzzle (const time_t);
        void set_starting_positions (uint8_t);
        void set_starting_positions (const uint8_t[NUM_CONTAINERS][NUM_CONTAINERS]);
        void create_map ();
        bool solve (const uint8_t, const uint8_t, Row[NUM_CONTAINERS], Column[NUM_CONTAINERS],
                    Box[NUM_CONTAINERS]);
        uint8_t at (const uint8_t);

    public:
        Grid () {}  //NOTE: Default constructor. Does nothing.
        Grid (const uint8_t[NUM_CONTAINERS][NUM_CONTAINERS]);
        Grid (const difficulty_level);
        ~Grid () {}  //NOTE: Default destructor. Does nothing.
        
        uint8_t map_box (const uint8_t, const uint8_t);
        uint8_t map_row (const uint8_t);
        uint8_t map_column (const uint8_t);
        Box& get_box (const uint8_t);
        Row& get_row (const uint8_t);
        Column& get_column (const uint8_t);
        uint8_t get_row_index (const uint8_t);
        uint8_t get_column_index (const uint8_t);
        uint8_t get_box_index (const uint8_t);
        
        #if DEBUG
        void mvprintw (const cell, const bool, const bool);
        void printw (const bool, const bool);
        #endif
        
        const cell get_position (const uint8_t);
        uint8_t get_map_size () const;
        uint8_t operator [] (const uint8_t);
        bool is_known (uint8_t);
        bool evaluate ();
};

#endif
