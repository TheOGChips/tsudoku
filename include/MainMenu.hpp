#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "Menu.hpp"

enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS };   //TODO: Add EXIT option

class MainMenu : public Menu {
    private:
        void display_menu (const uint8_t, const uint8_t, const options) override;
        
    public:        
        MainMenu ();
        ~MainMenu ();
        options menu () override;
        //friend options operator ++ (options&);
        //friend options operator -- (options&);
};

options operator ++ (options&);
options operator -- (options&);

#endif
