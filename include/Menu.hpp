#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>

enum class options { NEW_GAME, RESUME_GAME, SHOW_STATS };
enum class menu_type { MAIN, IN_GAME };

class Menu {
    private:
        menu_type type;
        void display_main_menu (options, uint8_t, uint8_t);
        
    public:
        Menu (const menu_type);
        ~Menu ();
        options main_menu ();
};

#endif
