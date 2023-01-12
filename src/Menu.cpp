#include <ncurses.h>
#include "Menu.hpp"
#include "colors.hpp"
#include <sstream>
#include "values.hpp"

Menu::Menu () {
    //TODO: This is only required if using the main menu, will need to account for this later (look at what I did for Sudoku)
    initscr();
    cbreak();   //TODO: Will need to account for signal handling
    noecho();
    keypad(stdscr, true);
    
    start_color();
    init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
}

Menu::~Menu () {
    echo();
    nocbreak();
    endwin();
}

void Menu::display_main_menu (options opt, uint8_t y, uint8_t x) {
    string opt1 = "New Game",
           opt2 = "Resume Game",
           opt3 = "Show # Finished Games";
           
    mvprintw(y/2 - 2, x/2 - 5, "MAIN MENU");
    if (opt == options::NEW_GAME) {
        attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2,     x/2 - 5, opt1.c_str());
        attroff(COLOR_PAIR(MENU_SELECTION));
        mvprintw(y/2 + 1, x/2 - 5, opt2.c_str());
        mvprintw(y/2 + 2, x/2 - 5, opt3.c_str());
    }
    else if (opt == options::RESUME_GAME) {
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

options operator ++ (options& opt) {
    return opt = (opt == options::NEW_GAME) ? options::RESUME_GAME : options::SHOW_STATS;
}

options operator -- (options& opt) {
    return opt = (opt == options::SHOW_STATS) ? options::RESUME_GAME : options::NEW_GAME;
}

options Menu::main_menu () {
    uint8_t x_max,
            y_max,
            ORIGINfirst = 3,
            ORIGINsecond = 6,
            y_min = ORIGINy * 2 + 30,  //TODO: Change min size in order to fit the in-game menu
            x_min = ORIGINx * 2 + 27;
    getmaxyx(stdscr, y_max, x_max);
    while (y_max < y_min or x_max < x_min) {
        uint8_t x_curr,
                y_curr;
        clear();
        string msg1 = "The current window is too small",
               msg4 = "Resize the terminal window and press Enter to continue";
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
    options opt = options::NEW_GAME;
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
