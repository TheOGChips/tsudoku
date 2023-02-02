#include <ncurses.h>
#include "MainMenu.hpp"
#include "colors.hpp"
#include <sstream>
#include <map>

using namespace std;

MainMenu::MainMenu () {
    initscr();  //TODO: Account for already having been called somehow
    cbreak();   //TODO: Will need to account for signal handling
    noecho();
    keypad(stdscr, true);
    
    start_color();
    init_pair(MENU_SELECTION, COLOR_BLACK, COLOR_WHITE);
}

MainMenu::~MainMenu () {
    echo();
    nocbreak();
    endwin();
}

void MainMenu::display_menu (const uint8_t Y, const uint8_t X, const options OPT) {
    const string TITLE = "MAIN MENU";
    const uint8_t Y_CENTER = Y / 2,
                  X_CENTER = X/2 - TITLE.size()/2,
                  NUM_OPTS = 3;
    const string OPTS[NUM_OPTS] = { "New Game", "Resume Game", "Show # Finished Games" };
    map<uint8_t, options> opt_map;
    opt_map[0] = options::NEW_GAME;
    opt_map[1] = options::RESUME_GAME;
    opt_map[2] = options::SHOW_STATS;
           
    mvprintw(Y_CENTER - 2, X_CENTER, "%s", TITLE.c_str());
    for (uint8_t i = 0; i < NUM_OPTS; i++) {
        if (OPT == opt_map[i]) attron(COLOR_PAIR(MENU_SELECTION));
        mvprintw(Y_CENTER + i, X_CENTER, "%s", OPTS[i].c_str());
        if (OPT == opt_map[i]) attroff(COLOR_PAIR(MENU_SELECTION));
    }
    refresh();
}

void MainMenu::set_VERTICAL_DIVIDER (const uint8_t VAL) {
    VERTICAL_DIVIDER = VAL;
}

void MainMenu::set_IN_GAME_MENU_DISPLAY_SPACING (const uint8_t VAL) {
    IN_GAME_MENU_DISPLAY_SPACING = VAL;
}

void MainMenu::set_WINDOW_XMIN () {
    WINDOW_XMIN = LEFT_PADDING + PUZZLE_SPACE + VERTICAL_DIVIDER + IN_GAME_MENU_DISPLAY_SPACING +
                  RIGHT_PADDING;
}

options MainMenu::menu (const bool USE_IN_GAME_MENU) {
    if (not USE_IN_GAME_MENU) {
        set_VERTICAL_DIVIDER(0);
        set_IN_GAME_MENU_DISPLAY_SPACING(0);
    }
    set_WINDOW_XMIN();
    
    return menu();
}

options MainMenu::menu () {
    uint8_t x_max,
            y_max;
    getmaxyx(stdscr, y_max, x_max);
    while (y_max < WINDOW_YMIN or x_max < WINDOW_XMIN) {
        uint8_t x_curr,
                y_curr;
        ::clear();
        string msg1 = "The current window is too small",
               msg4 = "Resize the terminal window and press Enter twice to continue";
        stringstream msg2,
                     msg3;
        msg2 << "Required dimensions: " << WINDOW_XMIN+0 << " x " << WINDOW_YMIN+0;
        msg3 << "Current dimensions: " << x_max+0 << " x " << y_max+0;
        mvprintw(y_max/2,     x_max/2 - msg1.size()/2,       "%s", msg1.c_str());
        mvprintw(y_max/2 + 2, x_max/2 - msg2.str().size()/2, "%s", msg2.str().c_str());
        mvprintw(y_max/2 + 3, x_max/2 - msg3.str().size()/2, "%s", msg3.str().c_str());
        mvprintw(y_max/2 + 5, x_max/2 - msg4.size()/2,       "%s", msg4.c_str());
        refresh();
        getmaxyx(stdscr, y_max, x_max);
        while (getch() != KEY_ENTER);   //NOTE: For some reason, the Enter key needs to be pressed
    }                                   //      twice here
    ::clear();
    
    string msg1 = "The window is now an appropriate size",
           msg2 = "Press Enter to continue";
    mvprintw(y_max/2,     x_max/2 - msg1.size()/2, "%s", msg1.c_str());
    mvprintw(y_max/2 + 1, x_max/2 - msg2.size()/2, "%s", msg2.c_str());
    refresh();
    while (getch() != KEY_ENTER);
    ::clear();
    
    curs_set(0);
    options opt = options::NEW_GAME;
    display_menu(y_max, x_max, opt);
    
    uint16_t input;
    do {
        input = getch();
        switch (input) {
            case KEY_DOWN:  ++opt;
                            display_menu(y_max, x_max, opt);
                            break;
                            
            case KEY_UP:    --opt;
                            display_menu(y_max, x_max, opt);
                            break;
                
            default:;
        }
    } while (input != KEY_ENTER);
    ::clear();
    curs_set(1);
    return opt;
}

options operator ++ (options& opt) {
    return opt = (opt == options::NEW_GAME) ? options::RESUME_GAME : options::SHOW_STATS;
}

options operator -- (options& opt) {
    return opt = (opt == options::SHOW_STATS) ? options::RESUME_GAME : options::NEW_GAME;
}
