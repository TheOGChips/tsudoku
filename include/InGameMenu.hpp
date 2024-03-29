#ifndef INGAMEMENU_HPP
#define INGAMEMENU_HPP

#include "Menu.hpp"

/* NOTE:
 * Class: InGameMenu
 * Purpose: Menu-derived class that displays the in-game menu and the content of the menu item
 *          chosen.
 * Private variables:
 *      IN_GAME_MENU_TITLE_SPACING -> The amount of spacing between the in-game menu title and its
 *                                    options.
 *      IN_GAME_MENU_LEFT_EDGE -> The leftmost column the display menu will occupy. This will be the
 *                                column immediately after the end of the vertical divider.
 *      NUM_OPTS -> The number of choosable options in the in-game menu.
 *      display_matrix -> A pointer to the display matrix used for saving the game. This must be a
 *                        pointer so that the display matrix will be properly updated in the in-game
 *                        menu's scope.
 */
class InGameMenu : public Menu {
    private:
        const uint8_t IN_GAME_MENU_TITLE_SPACING = 1,
                      IN_GAME_MENU_LEFT_EDGE = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER,
                      NUM_OPTS = 3;
        uint8_t* display_matrix[DISPLAY_MATRIX_COLUMNS];
        bool window_resized;
        
        void display_menu (const cell, const options) override;
        void clear (const cell);
        void display_rules (const cell);
        void display_manual (const cell);
        void screen_reader (const cell, std::string, uint8_t&);
        void save_game (const cell);
        void set_window_resized (const bool);
        
    public:
        InGameMenu (uint8_t[DISPLAY_MATRIX_ROWS][DISPLAY_MATRIX_COLUMNS]);
        ~InGameMenu () {}   //NOTE: Default destructor. Does nothing.
        options menu () override;
        static std::string save_game (uint8_t*[DISPLAY_MATRIX_COLUMNS]);
        bool get_window_resized () const;
};

#endif
