#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "Menu.hpp"

enum class mm_options { NEW_GAME, RESUME_GAME, SHOW_STATS };   //TODO: Add EXIT option

class MainMenu : public Menu {
    private:
        void display_menu (const uint8_t, const uint8_t, const uint8_t) override;
        
    public:
        //enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS };   //TODO: Add EXIT option
        
        MainMenu ();
        ~MainMenu ();
        uint8_t menu () override;
        //friend options operator ++ (options&);
        //friend options operator -- (options&);
};

mm_options operator ++ (mm_options&);
mm_options operator -- (mm_options&);

#endif
