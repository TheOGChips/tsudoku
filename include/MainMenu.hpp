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
 *      WINDOW_YMIN -> Minimum terminal window lines enforced on startup. If the window does not meet
 *                     this requirement, it will halt until the user resizes their terminal window.
 *      WINDOW_XMIN -> Minimum terminal window columns enforced on startup. This value is dependent
 *                     on whether the in-game menu is enable. If the window does not meet this
 *                     requirement, it will halt until the user resizes their terminal window.
 */
class MainMenu : public Menu {
    private:
        const uint8_t BOTTOM_PADDING = TOP_PADDING,
                      RIGHT_PADDING = LEFT_PADDING,
                      RESULT_MSG_SPACE = 3,
                      WINDOW_YMIN = TOP_PADDING + PUZZLE_SPACE + RESULT_MSG_SPACE + BOTTOM_PADDING;
        uint8_t WINDOW_XMIN;
        
        void display_menu (const uint8_t, const uint8_t, const options) override;
        void set_VERTICAL_DIVIDER (const uint8_t);
        void set_IN_GAME_MENU_DISPLAY_SPACING (const uint8_t);
        void set_WINDOW_XMIN ();
        
    public:        
        MainMenu ();    //NOTE: Default constructor. Does nothing.
        ~MainMenu ();   //NOTE: Default destructor. Does nothing.
        options menu () override;
        options menu (const bool);
};

#endif
