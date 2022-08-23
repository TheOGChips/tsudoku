#include "sudoku.hpp"
//#include <ncurses.h>

int main ()
{
    Sudoku puzzle;  //initialize a sudoku puzzle
    //TODO: Get user input to work correctly
    puzzle.printw();
    puzzle.refresh();
    //TODO: Start game
    puzzle.start_game();
    //clear();
    
    return 0;
}
