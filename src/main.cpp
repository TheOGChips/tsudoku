#include "Menu.hpp"
#include "sudoku.hpp"
#include <ncurses.h>
#include <filesystem>   //filesystem::create_directory, filesystem::exists
#include <fstream>      //std::ofstream

void create_dir ();

int main ()
{
    create_dir();
    
    Menu main_menu;
    options opt = main_menu.main_menu();
    
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

void create_dir () {
    using namespace std::filesystem;
    string HOME = getenv("HOME"),
           dir = HOME + "/.tsudoku",
           completed = dir + "/completed_puzzles.txt";
    create_directory(dir.c_str());
    
    if (not exists(completed.c_str())) {
        ofstream outfile;
        outfile.open(completed.c_str());
        outfile << 0 << endl;
        outfile.close();
    }
}
