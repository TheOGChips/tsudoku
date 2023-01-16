#ifndef INGAMEMENU_HPP
#define INGAMEMENU_HPP

#include "Menu.hpp"

enum class options { RULES, MANUAL, SAVE_GAME, NONE };

class InGameMenu : public Menu {
    private:
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
