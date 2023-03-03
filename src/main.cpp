/* NOTE: NCurses is included first in all files where its needed so that the KEY_ENTER
 *       redefinition in values.hpp persists across files the way I wanted.
 */
#include <ncurses.h>
#include "tsudoku.hpp"
#include "MainMenu.hpp" //      
#include "SavedGameMenu.hpp"
#include "Sudoku.hpp"   //NOTE: This inclusion here should cause issues with the getch() calls in
                        //      this file; however, there don't appear to be any for some reason.
#include <cstring>      //strcmp

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
 *                  3. "--delete-saved-games" or "-d" to delete all saved games
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
    #if DEBUG
        printw("sizeof(Sudoku): %lu B\n", sizeof(Sudoku));
        printw("sizeof(Sudoku): %.7f kB", sizeof(Sudoku) / 1024.0);
        refresh();
        getch();
    #endif
    return 0;
}
