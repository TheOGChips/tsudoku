#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <string>
#include "values.hpp"

enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS, EXIT, RULES, MANUAL, SAVE_GAME, SAVE_READY,
                     NO_SAVES, NONE };

class Menu {
    protected:
        const uint8_t TOP_PADDING = ORIGINy,    //blank space on all the edges of the window
                      LEFT_PADDING = ORIGINx,
                      PUZZLE_SPACE = 29;        //space (x and y) for the display puzzle
        uint8_t VERTICAL_DIVIDER = 4,     //blank space between display puzzle and in-game menu
                IN_GAME_MENU_DISPLAY_SPACING = 80;    //x-axis, columns for display area
                //TODO: Change this back to 90 for final release
        
        virtual void display_menu (const uint8_t, const uint8_t, const options) = PURE_VIRTUAL;
        
    public:
        Menu () {}
        ~Menu () {}
        virtual options menu () = PURE_VIRTUAL;
        //virtual options operator ++ ();
        //virtual options operator -- ();
};


#endif
