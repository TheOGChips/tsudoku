#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <string>
#include "values.hpp"

enum class options;

class Menu {
    protected:
        uint8_t VERTICAL_DIVIDER = 4,     //blank space between display puzzle and in-game menu
                IN_GAME_MENU_DISPLAY_SPACING = 90;    //x-axis, columns for display area
        
    public:
        //TODO: Fix this so that minimum window width adjusts accordingly for no in-game menu option
        const uint8_t TOP_PADDING = ORIGINy,    //blank space on all the edges of the window
                      LEFT_PADDING = ORIGINx,
                      PUZZLE_SPACE = 29;        //space (x and y) for the display puzzle
        
        Menu () {}
        ~Menu () {}
        virtual options menu () = PURE_VIRTUAL;
        virtual void display_menu (const uint8_t, const uint8_t, const options) = PURE_VIRTUAL;
        //virtual options operator ++ ();
        //virtual options operator -- ();
};


#endif
