#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>

enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS };

class Menu {
    private:
        void display_main_menu (options, uint8_t, uint8_t);
        
    public:
        Menu ();
        ~Menu ();
        options main_menu ();
};

#endif
