#include "sudoku.hpp"
#include <ncurses.h>

int main ()
{
    Matrix_9x9 puzzle = sudoku_init();  //initialize a sudoku puzzle
    //TODO: 
    //Start ncurses
    initscr();
    //establish color pairs
    start_color();
    init_pair(UNKNOWN, COLOR_BLACK, COLOR_WHITE);
    init_pair(KNOWN, COLOR_RED, COLOR_BLACK);
    init_pair(GUESS, COLOR_GREEN, COLOR_BLACK);
    //print puzzle and prompt
    printw("Printing by submatrix...\n");
    puzzle.printw(true, false);
    refresh();
    getch();
    clear();
    printw("Printing by column...\n");
    puzzle.printw(false, true);
    refresh();
    getch();
    clear();
    printw("Printing by row...\n");
    puzzle.printw(false, false);
    printw("\nPress any key to continue...");
    refresh();  //flush output to screen
    getch();    //wait for user input
    endwin();   //terminate ncurses session
    return 0;
}
