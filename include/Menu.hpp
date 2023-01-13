#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <string>

enum class main_options { NEW_GAME, RESUME_GAME, SHOW_STATS };
enum class in_game_options { RULES, MANUAL, SAVE_GAME, NONE };
enum class menu_type { MAIN, IN_GAME };

//TODO: This class would be a good instance to use polymorphism (I think)
class Menu {
    private:
        menu_type type;
        const uint8_t IN_GAME_MENU_TITLE_SPACING = 1,   //y-axis, blank lines below menu title
                      IN_GAME_MENU_DISPLAY_SPACING = 90;    //x-axis, columns for display area
        void display_main_menu (main_options, uint8_t, uint8_t);
        void display_in_game_menu (in_game_options, uint8_t, uint8_t);
        void clear (uint8_t, uint8_t);
        void display_rules (uint8_t, uint8_t);
        void display_manual (uint8_t, uint8_t);
        void screen_reader (std::string, uint8_t&, uint8_t, uint8_t);
        
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
