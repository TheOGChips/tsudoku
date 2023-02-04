#ifndef SAVEDGAMEMENU_HPP
#define SAVEDGAMEMENU_HPP

#include "Menu.hpp"
#include <filesystem>
#include <list>

//enum class options { NONE };

//NOTE: The address of a SavedPuzzle object and it's mat member matrix will be the same. This is here to make passing the matrix around easier.
struct SavedPuzzle {
    uint8_t puzzle[DISPLAY_MATRIX_SIZE][DISPLAY_MATRIX_SIZE];
    char color_codes[DISPLAY_MATRIX_SIZE][DISPLAY_MATRIX_SIZE];
};

class SavedGameMenu : public Menu {
    private:
        typedef std::list<std::string>::iterator list_iter;
        std::list<std::string> saved_games;
        list_iter selection;
        uint8_t saved_game[DISPLAY_MATRIX_SIZE][DISPLAY_MATRIX_SIZE] = {};
        char saved_color_codes[DISPLAY_MATRIX_SIZE][DISPLAY_MATRIX_SIZE] = {};
        
        void display_menu (const uint8_t, const uint8_t, const options) override;
        void generate_saved_games_list ();
        void select_saved_game ();
        void read_saved_game ();
        void print_saved_game ();
        
    public:
        SavedGameMenu () {} //TODO: Not having this defined in SavedGameMenu.cpp caused a weird crash before. I suspected it might have been some odd linking issue, and compiling from scratch seems to have fixed it. Delete this comment later if no other problems occur.
        ~SavedGameMenu () {}
        options menu () override;
        SavedPuzzle get_saved_game();
};

#endif
