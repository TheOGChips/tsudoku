#include "Menu.hpp"
#include "sudoku.hpp"
#include <ncurses.h>

int main ()
{
    Menu main_menu;
    options opt = main_menu.main_menu(); //TODO: Start in main menu
    
    if (opt == options::NEW_GAME) {
        Sudoku puzzle (true);  //initialize a sudoku puzzle
        puzzle.start_game();
    }
    else if (opt == options::RESUME_GAME);
    else;
    
    clear();
    printw("sizeof(Sudoku): %lu B\n", sizeof(Sudoku));
    printw("sizeof(Sudoku): %.7f kB", sizeof(Sudoku) / 1024.0);
    refresh();
    getch();
    return 0;
}
