#ifndef INGAMEMENU_HPP
#define INGAMEMENU_HPP

#include "Menu.hpp"

enum class options { RULES, MANUAL, SAVE_GAME, NONE };

class InGameMenu : public Menu {
    private:
        const uint8_t IN_GAME_MENU_TITLE_SPACING = 1,   //y-axis, blank lines below menu title
                      IN_GAME_MENU_LEFT_EDGE = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER;
        
        void display_menu (const uint8_t, const uint8_t, const options) override;
        void clear (const uint8_t, const uint8_t);
        void display_rules (const uint8_t, const uint8_t);
        void display_manual (const uint8_t, const uint8_t);
        void screen_reader (const uint8_t, const uint8_t, std::string, uint8_t&);
        
    public:
        InGameMenu () {}
        ~InGameMenu () {}
        options menu () override;
};

options operator ++ (options&, int);
options operator -- (options&, int);

#endif