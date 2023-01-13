#include <ncurses.h>
#include "Menu.hpp"
#include "colors.hpp"
#include <sstream>
#include "values.hpp"

Menu::Menu (const menu_type type) {
    this->type = type;
    if (this->type == menu_type::MAIN) {
        initscr();
        cbreak();   //TODO: Will need to account for signal handling
        noecho();
        keypad(stdscr, true);
        
        start_color();
        init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
    }
}

Menu::~Menu () {
    if (type == menu_type::MAIN) {
        echo();
        nocbreak();
        endwin();
    }
}

void Menu::display_main_menu (main_options opt, uint8_t y, uint8_t x) {
    string opt1 = "New Game",
           opt2 = "Resume Game",
           opt3 = "Show # Finished Games";
           
    mvprintw(y/2 - 2, x/2 - 5, "MAIN MENU");
    if (opt == main_options::NEW_GAME) {
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2,     x/2 - 5, opt1.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2 + 1, x/2 - 5, opt2.c_str());
        mvprintw(y/2 + 2, x/2 - 5, opt3.c_str());
    }
    else if (opt == main_options::RESUME_GAME) {
        mvprintw(y/2,     x/2 - 5, opt1.c_str());
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2 + 1, x/2 - 5, opt2.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2 + 2, x/2 - 5, opt3.c_str());
    }
    else {
        mvprintw(y/2,     x/2 - 5, opt1.c_str());
        mvprintw(y/2 + 1, x/2 - 5, opt2.c_str());
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2 + 2, x/2 - 5, opt3.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
    }
}

void Menu::display_in_game_menu (in_game_options opt, uint8_t y, uint8_t x) {
    string opt1 = "View the rules of sudoku",
           opt2 = "See game manual",
           opt3 = "Save current game";
           
    mvprintw(y, x, "IN-GAME MENU");
    if (opt == in_game_options::RULES) {
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 1, x, opt1.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 2, x, opt2.c_str());
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 3, x, opt3.c_str());
    }
    else if (opt == in_game_options::MANUAL) {
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 1, x, opt1.c_str());
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 2, x, opt2.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 3, x, opt3.c_str());
    }
    else if (opt == in_game_options::SAVE_GAME) {
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 1, x, opt1.c_str());
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 2, x, opt2.c_str());
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 3, x, opt3.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
    }
    else {
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 1, x, opt1.c_str());
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 2, x, opt2.c_str());
        mvprintw(y + IN_GAME_MENU_TITLE_SPACING + 3, x, opt3.c_str());
    }
    //Options
    //Sudoku rules
    //Game instructions
    //Save game
    refresh();
}

void Menu::display_rules (uint8_t y_edge, uint8_t x_edge) {
    const string TITLE = "RULES FOR PLAYING SUDOKU",
                 RULES_INTRO = string("Sudoku is a puzzle game using the numbers 1-9. The ") +
                               "puzzle board is a 9x9 grid that can be broken up evenly in 3 " +
                               "different ways: rows, columns, and 3x3 sub-grids. A completed " +
                               "sudoku puzzle is one where each cell contains a number, but with " +
                               "the following restrictions:",
                 RULES_ROWS = "1. Each row can only contain one each of the numbers 1-9",
                 RULES_COLUMNS = "2. Each column can only contain one each of the numbers 1-9",
                 RULES_SUBMATRIX = "3. Each sub-grid can only contain one each of the numbers 1-9";
                 
    string rules[4] = { RULES_INTRO, RULES_ROWS, RULES_COLUMNS, RULES_SUBMATRIX };
    uint8_t display_line = 5;
    
    mvprintw(y_edge + IN_GAME_MENU_TITLE_SPACING + display_line++, x_edge, TITLE.c_str());
    for (uint8_t i = 0; i < 4; i++) {
        display_line++;
        screen_reader(rules[i], display_line, y_edge, x_edge);
    }
}

void Menu::screen_reader (string str, uint8_t& display_line, uint8_t y_edge, uint8_t x_edge) {
    string display_str;
    while (not str.empty()) {
        size_t space_pos = str.find_first_of(' ');
        
        if (space_pos == string::npos) {
            display_str += str;
            str.clear();
            mvprintw(y_edge + IN_GAME_MENU_TITLE_SPACING + display_line++, x_edge,
                     display_str.c_str());
        }
        else if (display_str.size() + space_pos + 1 < IN_GAME_MENU_DISPLAY_SPACING - 3) {
            display_str += str.substr(0, space_pos + 1);
            str = str.substr(space_pos + 1);
        }
        else {
            mvprintw(y_edge + IN_GAME_MENU_TITLE_SPACING + display_line++, x_edge,
                     display_str.c_str());
            display_str.clear();
        }
    }
    display_str.clear();
}

main_options Menu::main_menu () {
    uint8_t x_max,
            y_max,
            puzzle_space = 27,
            result_msg_space = 5,
            y_min = ORIGINy * 2 + puzzle_space + result_msg_space,  //TODO: Change min size in order to fit the in-game menu
            x_min = ORIGINx * 2 + puzzle_space + IN_GAME_MENU_DISPLAY_SPACING;
    getmaxyx(stdscr, y_max, x_max);
    while (y_max < y_min or x_max < x_min) {
        uint8_t x_curr,
                y_curr;
        clear();
        string msg1 = "The current window is too small",
               msg4 = "Resize the terminal window and press Enter twice to continue";
        stringstream msg2,
                     msg3;
        msg2 << "Required dimensions: " << x_min+0 << " x " << y_min+0;
        msg3 << "Current dimensions: " << x_max+0 << " x " << y_max+0;
        mvprintw(y_max/2,     x_max/2 - msg1.size()/2,       msg1.c_str());
        mvprintw(y_max/2 + 2, x_max/2 - msg2.str().size()/2, msg2.str().c_str());
        mvprintw(y_max/2 + 3, x_max/2 - msg3.str().size()/2, msg3.str().c_str());
        mvprintw(y_max/2 + 5, x_max/2 - msg4.size()/2,       msg4.c_str());
        refresh();
        getmaxyx(stdscr, y_max, x_max);
        while (getch() != KEY_ENTER);
    }
    clear();
    
    //const uint8_t KEY_ENTER = 10;
    string msg1 = "The window is now an appropriate size",
           msg2 = "Press Enter to continue";
    mvprintw(y_max/2,     x_max/2 - msg1.size()/2, msg1.c_str());
    mvprintw(y_max/2 + 1, x_max/2 - msg2.size()/2, msg2.c_str());
    refresh();
    while (getch() != KEY_ENTER);
    clear();
    
    curs_set(0);
    main_options opt = main_options::NEW_GAME;
    display_main_menu(opt, y_max, x_max);
    
    uint16_t input;
    do {
        refresh();
        input = getch();
        switch (input) {
            case KEY_DOWN:  display_main_menu(++opt, y_max, x_max);
                            break;
                            
            case KEY_UP:    display_main_menu(--opt, y_max, x_max);
                            break;
                
            default:;
        }
    } while (input != KEY_ENTER);
    clear();
    curs_set(1);
    return opt;
}

void Menu::in_game_menu () {
    uint8_t y_edge = ORIGINy,
            in_game_menu_edge = 5,
            x_edge = ORIGINx * 2 + 27 + in_game_menu_edge;
    in_game_options opt = in_game_options::RULES;
    
    curs_set(0);
    uint16_t input;
    do {
        refresh();
        display_in_game_menu(opt, y_edge, x_edge);
        input = getch();
        switch (input) {
            case KEY_DOWN:  ++opt;
                            break;
                           
            case KEY_UP:    --opt;
                            break;
                            
            //TODO: Case for Enter key
            case KEY_ENTER: if (opt == in_game_options::RULES) {
                                display_rules(y_edge, x_edge);
                            }
                            else if (opt == in_game_options::MANUAL) {
                                //TODO
                            }
                            else {  //NOTE: opt will never be NONE based on this logic
                                //TODO
                            }
            default:;
        }
    } while (tolower(input) != 'm');
    display_in_game_menu(in_game_options::NONE, y_edge, x_edge);
    curs_set(1);
}

main_options operator ++ (main_options& opt) {
    return opt = (opt == main_options::NEW_GAME) ? main_options::RESUME_GAME : main_options::SHOW_STATS;
}

main_options operator -- (main_options& opt) {
    return opt = (opt == main_options::SHOW_STATS) ? main_options::RESUME_GAME : main_options::NEW_GAME;
}

in_game_options operator ++ (in_game_options& opt) {
    return opt = (opt == in_game_options::RULES) ? in_game_options::MANUAL : in_game_options::SAVE_GAME;
}

in_game_options operator -- (in_game_options& opt) {
    return opt = (opt == in_game_options::SAVE_GAME) ? in_game_options::MANUAL : in_game_options::RULES;
}
