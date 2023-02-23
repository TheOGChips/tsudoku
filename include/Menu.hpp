#ifndef MENU_HPP
#define MENU_HPP

#include "misc.hpp"

/* NOTE: Options used by the various Menu-derived classes. There is probably a slightly better way to
 *       implement this had I been able to predict all the different types of menus I would end up
 *       needing ahead of time. I thought I would only need 2, but wound up needing 4. Originally,
 *       each derived class would only know of its specific options, but that became too restrictive
 *       for the implementation I already had designed.
 */
enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS, EXIT, RULES, MANUAL, SAVE_GAME, SAVE_READY,
                     NO_SAVES, NONE };
                     
/* NOTE:
 * Class: Menu
 * Purpose: Abstract class from which all Menu-derived classes inherit as a parent class.
 * Protected variables:
 *      TOP_PADDING -> Empty space between the top of the terminal window and the in-terminal
 *                     display.
 *      LEFT_PADDING -> Empty space between the left side of the terminal window and the in-terminal
 *                      display.
 *      PUZZLE_SPACE -> Minimum number of lines and columns required to properly display the
 *                      terminal (27 for all cells of the display matrix, 2 for border lines/columns
 *                      between boxes).
 *      VERTICAL_DIVIDER -> Empty space between puzzle and the in-game menu. Disabling the in-game
 *                          menu changes the default value.
 *      IN_GAME_MENU_DISPLAY_SPACING -> Number of columns to be used for the in-game menu display
 *                                      area. Disabling the in-game menu changes the default value.
 */
class Menu {
    protected:
        const uint8_t TOP_PADDING = ORIGINy,
                      LEFT_PADDING = ORIGINx,
                      PUZZLE_SPACE = 29;
        uint8_t VERTICAL_DIVIDER = 4,
                IN_GAME_MENU_DISPLAY_SPACING = 80;
        
        virtual void display_menu (const cell, const options) = PURE_VIRTUAL;
        
    public:
        Menu ();
        ~Menu () {} //NOTE: Default destructor. Does nothing.
        virtual options menu () = PURE_VIRTUAL;
        static void SIGINT_handler (int32_t);
        //TODO: The Menu classes still need a way to handle screen resizing (at least the InGameMenu)
};

#endif
