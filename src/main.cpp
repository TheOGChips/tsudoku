#include "Menu.hpp"
#include "sudoku.hpp"
#include <ncurses.h>
#include <filesystem>   //filesystem::create_directory, filesystem::exists
#include <fstream>      //std::ofstream, std::ifstream
#include <sstream>

#undef KEY_ENTER
const uint8_t KEY_ENTER = 10;

string HOME = getenv("HOME"),
           dir = HOME + "/.tsudoku",
           completed = dir + "/completed_puzzles.txt";
           
void create_dir ();
void display_completed_puzzles ();

int main ()
{
    create_dir();
    
    Menu main_menu;
    options opt = main_menu.main_menu();
    
    if (opt == options::NEW_GAME) {
        Sudoku puzzle (true);  //initialize a sudoku puzzle
        puzzle.start_game();
    }
    else if (opt == options::RESUME_GAME);  //TODO
    else {  //TODO
        display_completed_puzzles();
    }
    
    clear();
    printw("sizeof(Sudoku): %lu B\n", sizeof(Sudoku));
    printw("sizeof(Sudoku): %.7f kB", sizeof(Sudoku) / 1024.0);
    refresh();
    getch();
    return 0;
}

void create_dir () {
    using namespace std::filesystem;
    create_directory(dir.c_str());
    
    if (not exists(completed.c_str())) {
        ofstream outfile;
        outfile.open(completed.c_str());
        outfile << 0 << endl;
        outfile.close();
    }
}

void display_completed_puzzles () {
    uint8_t y_max,
            x_max;
    uint64_t num_completed;
    
    ifstream infile;
    infile.open(completed.c_str());
    infile >> num_completed;
    infile.close();
    
    stringstream sstr;
    sstr << "Completed Sudoku puzzles: " << num_completed;
    string str = "Press Enter to continue";
    getmaxyx(stdscr, y_max, x_max);
    clear();
    mvprintw(y_max/2, x_max/2 - sstr.str().size()/2, sstr.str().c_str());
    mvprintw(y_max/2 + 2, x_max/2 - str.size()/2, str.c_str());
    refresh();
    //getch();
    while (getch() != KEY_ENTER);
}
