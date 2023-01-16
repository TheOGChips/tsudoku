#ifndef INGAMEMENU_HPP
#define INGAMEMENU_HPP

#include "Menu.hpp"

enum class igm_options { RULES, MANUAL, SAVE_GAME, NONE };

class InGameMenu : public Menu {
    private:
        void display_menu (const uint8_t, const uint8_t, const uint8_t) override;
        void clear (const uint8_t, const uint8_t);
        void display_rules (const uint8_t, const uint8_t);
        void display_manual (const uint8_t, const uint8_t);
        void screen_reader (const uint8_t, const uint8_t, std::string, uint8_t&);
        
    public:
        InGameMenu () {}
        ~InGameMenu () {}
        uint8_t menu () override;
};

igm_options operator ++ (igm_options&);
igm_options operator -- (igm_options&);

#endif
