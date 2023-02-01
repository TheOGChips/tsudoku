#ifndef SAVEDGAMEMENU_HPP
#define SAVEDGAMEMENU_HPP

#include "Menu.hpp"
#include <filesystem>
#include <list>

//enum class options { NONE };

class SavedGameMenu : public Menu {
    private:
        std::list<std::string> saved_games;
        void display_menu (const uint8_t, const uint8_t, const options) override;
        void generate_saved_games_list ();
        
    public:
        SavedGameMenu () {} //TODO: Not having this defined in SavedGameMenu.cpp caused a weird crash before. I suspected it might have been some odd linking issue, and compiling from scratch seems to have fixed it. Delete this comment later if no other problems occur.
        ~SavedGameMenu () {}
        options menu () override;
};

#endif
