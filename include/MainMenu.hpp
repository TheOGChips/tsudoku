#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "Menu.hpp"

enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS };   //TODO: Add EXIT option

class MainMenu : public Menu {
    private:
        const uint8_t BOTTOM_PADDING = TOP_PADDING, //blank space on all the edges of the window
                      RIGHT_PADDING = LEFT_PADDING,
                      RESULT_MSG_SPACE = 3;     //space for the evaluation result to appear
        const uint8_t WINDOW_YMIN = TOP_PADDING + PUZZLE_SPACE + RESULT_MSG_SPACE + BOTTOM_PADDING,
                      WINDOW_XMIN = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER +
                                    IN_GAME_MENU_DISPLAY_SPACING + RIGHT_PADDING;
        
        void display_menu (const uint8_t, const uint8_t, const options) override;
        
    public:        
        MainMenu ();
        ~MainMenu ();
        options menu () override;
        options menu (bool);
        //friend options operator ++ (options&);
        //friend options operator -- (options&);
};

options operator ++ (options&);
options operator -- (options&);

#endif
