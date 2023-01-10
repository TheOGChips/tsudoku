#include "sudoku.hpp"
#include <ncurses.h>

int main ()
{
    Sudoku puzzle;  //initialize a sudoku puzzle
    puzzle.main_menu(); //TODO: Start in main menu
    puzzle.start_game();
    clear();
    printw("sizeof(puzzle): %lu B\n", sizeof(puzzle));
    printw("sizeof(puzzle): %.7f kB", sizeof(puzzle) / 1024.0);
    refresh();
    getch();
    return 0;
}
