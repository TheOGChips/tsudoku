#include "sudoku.hpp"
#include <ncurses.h>

int main ()
{
    Sudoku puzzle;  //initialize a sudoku puzzle
    //TODO: Get user input to work correctly
    puzzle.printw();
    refresh();
    getch();
    clear();
    
    return 0;
}
