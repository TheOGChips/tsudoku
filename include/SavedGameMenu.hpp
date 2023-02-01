#ifndef SAVEDGAMEMENU_HPP
#define SAVEDGAMEMENU_HPP

#include "Menu.hpp"
//#include <filesystem>

//enum class options { NONE };

class SavedGameMenu : public Menu {
    private:
        void display_menu (const uint8_t, const uint8_t, const options) override;
        
    public:
        SavedGameMenu () {}
        ~SavedGameMenu () {}
        options menu () override;
};

#endif
