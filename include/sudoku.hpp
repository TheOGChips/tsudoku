#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "Matrix_9x9.hpp"   //cell, std::map, std::utility
#include <cstdint>          //uint8_t

using namespace std;

class Sudoku {
    private:
        uint8_t display_matrix[27][27] = {};
        Matrix_9x9 mat;
        map<uint8_t, cell> _map_;   //maps 81 positions to positions in 27x27 display matrix
        /*const uint8_t INIT_OFFSETX = 6,
                      INIT_OFFSETY = 3;
        const cell ORIGIN = {INIT_OFFSETY, INIT_OFFSETX};*/
        const cell ORIGIN = {3, 6}; //NOTE: terminal coordinates are in (y,x) format
        cell cursor_pos;

        map<uint8_t, cell> create_map();
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
        
    public:
        Sudoku();
        ~Sudoku();

        //TODO: Start the game
        void start_game();
};

#endif
