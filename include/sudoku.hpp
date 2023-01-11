#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "Matrix_9x9.hpp"   //cell, std::map, std::utility
#include <cstdint>          //uint8_t
#include <array>            //std::array

using namespace std;

class Sudoku {
    private:
        uint8_t display_matrix[27][27] = {};
        Matrix_9x9 mat;
        map<uint8_t, cell> _map_;   //maps 81 positions to positions in 27x27 display matrix
        map<cell, uint8_t> _rev_map_;
        /*const uint8_t INIT_OFFSETX = 6,
                      INIT_OFFSETY = 3;
        const cell ORIGIN = {INIT_OFFSETY, INIT_OFFSETX};*/
        const cell ORIGIN = {3, 6}; //NOTE: terminal coordinates are in (y,x) format
        cell cursor_pos;
        map<cell, cell> display_matrix_offset;
        enum border_positions {TL = 0, T, TR, L, R, BL, B, BR, NUM_BORDER_POSITIONS};
        //const uint8_t NUM_POSITIONS = 8;

        //map<uint8_t, cell> create_map();
        void create_map();
        void map_display_matrix_offset (const uint8_t, const uint8_t);
        void set_color_pairs();
        //TODO: Start menu, accessed from constructor before generating a new puzzle.
        //      Can resume saved puzzle instead.
        void init_display_matrix();
        void printw(/*const bool, const bool*/);
        void move (const uint8_t, const uint8_t);
        void move (const uint16_t);
        void refresh();
        uint16_t getch();
        void clear();
        bool is_border (const uint8_t, const uint8_t);
        array<cell, NUM_BORDER_POSITIONS> get_surrounding_cells();
        bool do_nothing();
        void clear_surrounding_cells();
        void place_value (const uint16_t);
        void reset_cursor();
        bool evaluate();
        
    public:
        Sudoku (bool);
        ~Sudoku();

        void start_game();
};

#endif
