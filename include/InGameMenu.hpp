#ifndef INGAMEMENU_HPP
#define INGAMEMENU_HPP

#include "Menu.hpp"

//enum class options { RULES, MANUAL, SAVE_GAME, NONE };

class InGameMenu : public Menu {
    private:
        const uint8_t IN_GAME_MENU_TITLE_SPACING = 1,   //y-axis, blank lines below menu title
                      IN_GAME_MENU_LEFT_EDGE = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER,
                      NUM_OPTS = 3;
        uint8_t* display_matrix[DISPLAY_MATRIX_COLUMNS];    //NOTE: This must be a pointer so that the display matrix will properly update when saving a game in the in-game menu.
        
        void display_menu (const cell, const options) override;
        void clear (const uint8_t, const uint8_t);
        void display_rules (const uint8_t, const uint8_t);
        void display_manual (const uint8_t, const uint8_t);
        void screen_reader (const uint8_t, const uint8_t, std::string, uint8_t&);
        void save_game (const uint8_t, const uint8_t);
        
    public:
        InGameMenu (uint8_t[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS]);
        ~InGameMenu () {}
        options menu () override;
        static std::string save_game (uint8_t*[DISPLAY_MATRIX_COLUMNS]);
};

options operator ++ (options&, int);
options operator -- (options&, int);

#endif
