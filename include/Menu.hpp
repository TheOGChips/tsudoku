#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <string>
#include "values.hpp"

enum class main_options { NEW_GAME, RESUME_GAME, SHOW_STATS };
enum class in_game_options { RULES, MANUAL, SAVE_GAME, NONE };
enum class menu_type { MAIN, IN_GAME };

/*
 * TODO: This class would be a good instance to use polymorphism (look up virtual functions). Look
 *       into this next.
 */
class Menu {
    private:
        menu_type type;
        const uint8_t TOP_PADDING = ORIGINy,    //blank space on all the edges of the window
                      LEFT_PADDING = ORIGINx,
                      BOTTOM_PADDING = TOP_PADDING,
                      RIGHT_PADDING = LEFT_PADDING,
                      PUZZLE_SPACE = 29,        //space (x and y) for the display puzzle
                      VERTICAL_DIVIDER = 4,     //blank space between display puzzle and in-game menu
                      RESULT_MSG_SPACE = 3,     //space for the evaluation result to appear
                      IN_GAME_MENU_LEFT_EDGE = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER,
                      IN_GAME_MENU_TITLE_SPACING = 1,   //y-axis, blank lines below menu title
                      IN_GAME_MENU_DISPLAY_SPACING = 90,    //x-axis, columns for display area
                      WINDOW_YMIN = TOP_PADDING + PUZZLE_SPACE + RESULT_MSG_SPACE + BOTTOM_PADDING,
                      WINDOW_XMIN = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER +
                                    IN_GAME_MENU_DISPLAY_SPACING + RIGHT_PADDING;
        void display_main_menu (const uint8_t, const uint8_t, main_options);
        void display_in_game_menu (const uint8_t, const uint8_t, in_game_options);
        void clear (const uint8_t, const uint8_t);
        void display_rules (const uint8_t, const uint8_t);
        void display_manual (const uint8_t, const uint8_t);
        void screen_reader (const uint8_t, const uint8_t, std::string, uint8_t&);
        
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
