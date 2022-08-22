#include "sudoku.hpp"
#include <ncurses.h>

int main ()
{
    Sudoku puzzle;  //initialize a sudoku puzzle
    //TODO: Get 27x27 matrix to print out correctly
    puzzle.printw();
    refresh();
    getch();
    clear();
    
    return 0;
}
