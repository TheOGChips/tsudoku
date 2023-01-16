#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <string>
#include "values.hpp"

/*
 * TODO: This class would be a good instance to use polymorphism (look up virtual functions). Look
 *       into this next.
 */

//enum class options;

class Menu {
    public:
        const uint8_t TOP_PADDING = ORIGINy,    //blank space on all the edges of the window
                      LEFT_PADDING = ORIGINx,
                      BOTTOM_PADDING = TOP_PADDING,
                      RIGHT_PADDING = LEFT_PADDING,
                      PUZZLE_SPACE = 29,        //space (x and y) for the display puzzle
                      VERTICAL_DIVIDER = 4,     //blank space between display puzzle and in-game menu
                      RESULT_MSG_SPACE = 3,     //space for the evaluation result to appear
                      IN_GAME_MENU_LEFT_EDGE = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER,
                      IN_GAME_MENU_TITLE_SPACING = 1,   //y-axis, blank lines below menu title
                      IN_GAME_MENU_DISPLAY_SPACING = 90,    //x-axis, columns for display area
                      WINDOW_YMIN = TOP_PADDING + PUZZLE_SPACE + RESULT_MSG_SPACE + BOTTOM_PADDING,
                      WINDOW_XMIN = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER +
                                    IN_GAME_MENU_DISPLAY_SPACING + RIGHT_PADDING;
        
        Menu () {}
        ~Menu () {}
        virtual uint8_t menu () = PURE_VIRTUAL;
        virtual void display_menu (const uint8_t, const uint8_t, const uint8_t) = PURE_VIRTUAL;
        //virtual options operator ++ ();
        //virtual options operator -- ();
};


#endif
