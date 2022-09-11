#include "sudoku.hpp"
#include <ncurses.h>

int main ()
{
    Sudoku puzzle;  //initialize a sudoku puzzle
    //TODO: Get user input to work correctly
    //TODO: Start game
    puzzle.start_game();
    clear();
    printw("sizeof(puzzle): %d B\n", sizeof(puzzle));
    printw("sizeof(puzzle): %.7f kB", sizeof(puzzle) / 1024.0);
    refresh();
    getch();
    return 0;
}
