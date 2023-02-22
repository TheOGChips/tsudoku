#ifndef SAVEDGAMEMENU_HPP
#define SAVEDGAMEMENU_HPP

#include "Menu.hpp"
#include <filesystem>
#include <list>

/* NOTE:
 * Class: SavedGameMenu
 * Purpose: Menu-derived class that displays the a list of saved games the user can choose from to
 *          resume.
 * Private variables:
 *      list_iter -> Alias to a string list iterator.
 *      saved_games -> The list of saved games to be displayed to the screen. Each will appear as an
 *                     option the user can choose from.
 *      selection -> The currently highlighted game from the displayed list. If the user presses
 *                   Enter, this becomes the game loaded.
 *      saved_game -> The matrix used to read a saved game into. This later becomes the display
 *                    matrix.
 *      saved_color_codes -> The matrix used to read saved game display matrix color codes into. This
 *                           is used to display the saved game with the same coloring as the session
 *                           from when it was saved.
 */
class SavedGameMenu : public Menu {
    private:
        typedef std::list<std::string>::iterator list_iter;
        std::list<std::string> saved_games;
        list_iter selection;
        uint8_t saved_game[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS] = {};
        char saved_color_codes[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS] = {};
        
        void display_menu (const cell, const options) override;
        void generate_saved_games_list ();
        bool select_saved_game ();
        void read_saved_game ();
        void print_saved_game ();
        
    public:
        SavedGameMenu () {}     //NOTE: Default constructor. Does nothing.
        ~SavedGameMenu () {}    //NOTE: Default destructor. Does nothing.
        options menu () override;
        SavedPuzzle get_saved_game ();
};

#endif
