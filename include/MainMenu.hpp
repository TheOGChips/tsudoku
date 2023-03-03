#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "Menu.hpp"

/* NOTE:
 * Class: MainMenu
 * Purpose: Menu-derived class that displays and controls the main menu the user sees before and
 *          after every game.
 * Private variables:
 *      BOTTOM_PADDING -> Empty space between the bottom of the terminal window and the in-terminal
 *                        display.
 *      RIGHT_PADDING -> Empty space between the right side of the terminal window and the in
 *                       terminal display.
 *      RESULT_MSG_SPACE -> Minimum number of lines required to properly display the puzzle's
 *                          evaluation result.
 */
class MainMenu : public Menu {
    private:
        const uint8_t BOTTOM_PADDING = TOP_PADDING,
                      RIGHT_PADDING = LEFT_PADDING,
                      RESULT_MSG_SPACE = 3;
        
        void display_menu (const cell, const options) override;
        void set_VERTICAL_DIVIDER (const uint8_t);
        void set_IN_GAME_MENU_DISPLAY_SPACING (const uint8_t);
        void set_WINDOW_REQ ();
        
    public:        
        MainMenu ();
        ~MainMenu ();
        options menu () override;
        options menu (const bool);
};

#endif
