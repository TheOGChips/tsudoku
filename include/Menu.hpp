#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>

enum class main_options { NEW_GAME, RESUME_GAME, SHOW_STATS };
enum class in_game_options { RULES, MANUAL, SAVE_GAME, NONE };
enum class menu_type { MAIN, IN_GAME };

class Menu {
    private:
        menu_type type;
        void display_main_menu (main_options, uint8_t, uint8_t);
        void display_in_game_menu (in_game_options, uint8_t, uint8_t);
        
    public:
        Menu (const menu_type);
        ~Menu ();
        main_options main_menu ();
        void in_game_menu ();
};

main_options operator ++ (main_options&);
main_options operator -- (main_options&);
in_game_options operator ++ (in_game_options&);
in_game_options operator -- (in_game_options&);

#endif
