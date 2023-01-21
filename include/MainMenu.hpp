#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "Menu.hpp"

enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS };   //TODO: Add EXIT option

class MainMenu : public Menu {
    private:
        const uint8_t BOTTOM_PADDING = TOP_PADDING, //blank space on all the edges of the window
                      RIGHT_PADDING = LEFT_PADDING,
                      RESULT_MSG_SPACE = 3,     //space for the evaluation result to appear
                      WINDOW_YMIN = TOP_PADDING + PUZZLE_SPACE + RESULT_MSG_SPACE + BOTTOM_PADDING;
        uint8_t WINDOW_XMIN;
        
        void display_menu (const uint8_t, const uint8_t, const options) override;
        void set_VERTICAL_DIVIDER (const uint8_t);
        void set_IN_GAME_MENU_DISPLAY_SPACING (const uint8_t);
        void set_WINDOW_XMIN ();
        
    public:        
        MainMenu ();
        ~MainMenu ();
        options menu () override;
        options menu (const bool);
        //friend options operator ++ (options&);
        //friend options operator -- (options&);
};

options operator ++ (options&);
options operator -- (options&);

#endif
