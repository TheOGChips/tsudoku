#include <ncurses.h>    //NOTE: NCurses is included first in all the files here so that the KEY_ENTER
#include "MainMenu.hpp" //      redefinition in values.hpp persists across files the way I wanted.
#include "sudoku.hpp"   //NOTE: This inclusion here should cause issues with the getch() calls in
                        //      this file; however, there don't appear to be any for some reason.
#include <filesystem>   //filesystem::create_directory, filesystem::exists
#include <fstream>      //std::ofstream, std::ifstream
#include <sstream>      //std::stringstream
#include <cstring>      //strcmp

string HOME = getenv("HOME"),   //TODO: Make these const
       dir = HOME + "/.tsudoku",
       completed = dir + "/completed_puzzles.txt";
enum class err_msg { INVALID_ARG, TOO_MANY_ARGS };

void create_dir ();
void display_completed_puzzles ();
void print_err_msg (err_msg);

int main (int argc, char** argv) //TODO: The majority of this code will need to be in a loop
{
    bool use_in_game_menu;
    switch (argc) {
        case 1: use_in_game_menu = true;
                break;
                
        case 2: if (not strcmp(argv[1], "--no-in-game-menu") or
                    not strcmp(argv[1], "-n")) use_in_game_menu = false;
                //TODO: Add a --help/-? option
                //TODO: Add a --info option
                else {
                    print_err_msg(err_msg::INVALID_ARG);
                    return 1;
                }
                break;
                
        default: print_err_msg(err_msg::TOO_MANY_ARGS);
                 return 1;
    }
    
    //printf("argv[1]: %s\n", argv[1]);
    //return 0;
    create_dir();
    
    MainMenu main_menu;
    options opt = main_menu.menu(use_in_game_menu);
    
    switch (opt) {
        case options::NEW_GAME:  {
                                        Sudoku puzzle(true);    //TODO: Consider making this a
                                                                //      static function, depending
                                                                //      on how resuming games works
                                        puzzle.start_game(use_in_game_menu);
                                        break;
                                    }
                       
        case options::RESUME_GAME: break;  //TODO
        
        case options::SHOW_STATS: display_completed_puzzles();
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

void print_err_msg (err_msg err) {
    string str;
    switch (err) {
        case err_msg::INVALID_ARG: str = string("Invalid argument. Use the '--help' option to ") +
                                         "see a list of valid options.";
                                   break;
                          
        case err_msg::TOO_MANY_ARGS: str = string("Too many arguments. Use the '--help' option ") +
                                           "for a short how-to.";
                                     break;
                            
        default:;
    }
    printf("Error: %s\n", str.c_str());
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
