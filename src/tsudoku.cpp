#include <ncurses.h>
#include "tsudoku.hpp"
#include <filesystem>   //filesystem::create_directory, filesystem::exists
#include <fstream>      //std::ofstream, std::ifstream
#include <sstream>      //std::stringstream

using namespace std;

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
