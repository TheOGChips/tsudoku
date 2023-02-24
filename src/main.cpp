#include <ncurses.h>    //NOTE: NCurses is included first in all the files here so that the KEY_ENTER
#include "MainMenu.hpp" //      redefinition in values.hpp persists across files the way I wanted.
#include "SavedGameMenu.hpp"
#include "Sudoku.hpp"   //NOTE: This inclusion here should cause issues with the getch() calls in
                        //      this file; however, there don't appear to be any for some reason.
#include <filesystem>   //filesystem::create_directory, filesystem::exists
#include <fstream>      //std::ofstream, std::ifstream
#include <sstream>      //std::stringstream
#include <cstring>      //strcmp

//NOTE: This file keeps a persistent record of how many games the player has successfully finished
const std::string COMPLETED = DIR + "/completed_puzzles.txt";
enum class err_msg { INVALID_ARG, TOO_MANY_ARGS };  //NOTE: Possible types of errors that can be
                                                    //      generated
void create_dir ();
void display_completed_puzzles ();
void print_help ();
void print_err_msg (err_msg);
void delete_saved_games ();

/* NOTE:
 * Name: main
 * Purpose: The main function from the which the program starts. Tsudoku only accepts one option at
 *          run time.
 * Parameters:
 *      argc -> The total number of items entered on the command line. "tsudoku" will always be
 *              index 0 and any option, if provided, will start at index 1.
 *      argv -> The list of items entered on the command line. Possible tsudoku options are:
 *                  1. "--no-in-game-menu" or "-n" to disable the in-game menu
 *                  2. "--help" to display the help information
 */
int main (int argc, char** argv) {
    //NOTE: Detemine whether the in-game menu should be enabled and if there is any input error
    bool use_in_game_menu;
    switch (argc) {
        case 1:
            use_in_game_menu = true;
            break;
                
        case 2:
            if (not strcmp(argv[1], "--no-in-game-menu") or
                not strcmp(argv[1], "-n")) use_in_game_menu = false;
            else if (not strcmp(argv[1], "--help")) {
                print_help();
                return 0;
            }
            else if (not strcmp(argv[1], "--delete-saved-games") or
                     not strcmp(argv[1], "-d")) {
                delete_saved_games();
                return 0;
            }
            else {
                print_err_msg(err_msg::INVALID_ARG);
                return 1;
            }
            break;
                
        default:
            print_err_msg(err_msg::TOO_MANY_ARGS);
            return 1;
    }
    
    create_dir();   //NOTE: Create the tsudoku environment directory if it doesn't already exist
    
    MainMenu main_menu;
    options opt;
    do {
        opt = main_menu.menu(use_in_game_menu); //NOTE: Start the main menu and respond accordingly
        
        switch (opt) {
            case options::NEW_GAME: {   //NOTE: Start a new game
                Sudoku puzzle(nullptr);
                puzzle.start_game(use_in_game_menu, nullptr);
                break;
            }
                        
            case options::RESUME_GAME: {    //NOTE: Resume a previously saved game
                SavedGameMenu saved_game_menu;
                if (saved_game_menu.menu() == options::SAVE_READY) {
                    SavedPuzzle saved_puzzle = saved_game_menu.get_saved_game();
                    Sudoku puzzle(/*true, */&saved_puzzle);
                    puzzle.start_game(use_in_game_menu, &saved_puzzle);
                }
                break;
            }
            
            case options::SHOW_STATS:   //NOTE: Show how many games the player has finished
                display_completed_puzzles();
                break;
                            
            default:;
        }
    } while (opt != options::EXIT); //NOTE: Exit the program
    
    clear();
    printw("sizeof(Sudoku): %lu B\n", sizeof(Sudoku));  //TODO: Enclose this under DEBUG
    printw("sizeof(Sudoku): %.7f kB", sizeof(Sudoku) / 1024.0);
    refresh();
    getch();
    return 0;
}

/* NOTE:
 * Name: print_help
 * Purpose: Prints out how to run tsudoku when the player enters the --help option on the command
 *          line.
 * Parameters: None
 */
void print_help () {
    printf("\nUsage: $ ./tsudoku OPTION\n\n");
    printf("OPTIONS\n\n");
    printf("    -n, --no-in-game-menu\tRun tsudoku without the in-game menu\n");
    printf("    --help\t\t\tDisplay this help menu\n\n");
}

/* NOTE:
 * Name: print_err_msg
 * Purpose: Prints out an error message depending on the type of error the player committed.
 * Parameters:
 *      err -> The type of error the player committed. Possible options are:
 *                  1. invalid command line option (INVALID_ARG)
 *                  2. too many command line options (TOO_MANY_ARGS)
 */
void print_err_msg (err_msg err) {
    string str;
    switch (err) {
        case err_msg::INVALID_ARG:
            str = "Invalid argument. Use the '--help' option to see a list of valid options.";
            break;
            
        case err_msg::TOO_MANY_ARGS:
            str = "Too many arguments. Use the '--help' option for a short how-to.";
            break;
            
        default:;
    }
    printf("Error: %s\n", str.c_str());
}

/* NOTE:
 * Name: create_dir
 * Purpose: Creates the tsudoku environment directory in the user's home directory at ~/.tsudoku if
 *          it doesn't already exist.
 * Parameters: None
 */
void create_dir () {
    using namespace std::filesystem;    //NOTE: Use C++17's filesystem library
    create_directory(DIR.c_str());
    
    if (not exists(COMPLETED.c_str())) {
        ofstream outfile;
        outfile.open(COMPLETED.c_str());
        outfile << 0 << endl;
        outfile.close();
    }
}

/* NOTE:
 * Name: display_completed_puzzles
 * Purpose: Reads in the current number of games the player has successfully completed and then
 *          displays that information to the screen in the terminal window.
 * Parameters: None
 */
void display_completed_puzzles () {
    uint64_t num_completed;
    ifstream infile;
    infile.open(COMPLETED.c_str());
    infile >> num_completed;
    infile.close();
    
    stringstream sstr;
    sstr << "Completed Sudoku puzzles: " << num_completed;
    string str = "Press Enter to continue";
    
    uint8_t y_max,
            x_max;
    getmaxyx(stdscr, y_max, x_max);
    
    curs_set(false);    //NOTE: Turn cursor off while displaying information
    clear();
    mvprintw(y_max/2, x_max/2 - sstr.str().size()/2, "%s", sstr.str().c_str());
    mvprintw(y_max/2 + 2, x_max/2 - str.size()/2, "%s", str.c_str());
    refresh();
    while (getch() != KEY_ENTER);
    curs_set(true);     //NOTE: Turn cursor back on before returning to the calling function
}

/* NOTE:
 * Name: delete_saved_games
 * Purpose: Deletes all saved games from the tsudoku environment directory at ~/.tsudoku.
 * Parameters: None
 */
void delete_saved_games () {
    using namespace std::filesystem;
    typedef directory_iterator dir_iter;
    
    /* NOTE: The standard says it's not specified whether a directory_iterator is updated or not
     *       when a file is deleted, so this seems like a safer/more portable way to delete all
     *       CSV files.
     */
    dir_iter iter(DIR);
    while (iter != end(dir_iter())) {
        path filepath = iter->path();
        iter++;
        if (filepath.extension() == ".csv") {
            remove(filepath);
        }
    }
}
