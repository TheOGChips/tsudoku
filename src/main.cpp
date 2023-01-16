#include <ncurses.h>    //NOTE: NCurses is included first in all the files here so that the KEY_ENTER
#include "MainMenu.hpp" //      redefinition in values.hpp persists across files the way I wanted.
#include "sudoku.hpp"   //NOTE: This inclusion here should cause issues with the getch() calls in
                        //      this file; however, there don't appear to be any for some reason.
#include <filesystem>   //filesystem::create_directory, filesystem::exists
#include <fstream>      //std::ofstream, std::ifstream
#include <sstream>      //std::stringstream

string HOME = getenv("HOME"),   //TODO: Make these const
       dir = HOME + "/.tsudoku",
       completed = dir + "/completed_puzzles.txt";
           
void create_dir ();
void display_completed_puzzles ();

int main () //TODO: The majority of this code will need to be in a loop
{
    //printf("KEY_ENTER: %d\n", KEY_ENTER);
    //return 0;
    create_dir();
    
    MainMenu main_menu;
    mm_options opt = mm_options(main_menu.menu());
    /*
     * NOTE: The distinction between main menu and in-game menu options has to be maintained because
     *       the linker gets upset due to a "multiple definition" error for the operator overloads.
     *       Using actual functions instead of operator overloads might still work since the
     *       operators are prefix increment/decrement. This would help clean up the code some more,
     *       but loses the nice brevity of the operators. This might be done within the next 1 or 2
     *       commits if it works and depending on what I decide.
     */
    
    switch (opt) {
        case mm_options::NEW_GAME:  {
                                        Sudoku puzzle(true);    //TODO: Consider making this a static
                                        puzzle.start_game();    //      function, depending on how
                                        break;                  //      resuming games works
                                    }
                       
        case mm_options::RESUME_GAME: break;  //TODO
        
        case mm_options::SHOW_STATS: display_completed_puzzles();
                                     break;
                         
        default:;
    }
    
    clear();
    printw("sizeof(Sudoku): %lu B\n", sizeof(Sudoku));  //TODO: This can be deleted later
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
    uint64_t num_completed;
    ifstream infile;
    infile.open(completed.c_str());
    infile >> num_completed;
    infile.close();
    
    stringstream sstr;
    sstr << "Completed Sudoku puzzles: " << num_completed;
    string str = "Press Enter to continue";
    
    uint8_t y_max,
            x_max;
    getmaxyx(stdscr, y_max, x_max);
    
    clear();
    mvprintw(y_max/2, x_max/2 - sstr.str().size()/2, sstr.str().c_str());
    mvprintw(y_max/2 + 2, x_max/2 - str.size()/2, str.c_str());
    refresh();
    while (getch() != KEY_ENTER);
}
